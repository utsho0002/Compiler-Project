import os
import subprocess
import sys
import tempfile
from pathlib import Path

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

BASE_DIR = Path(__file__).resolve().parent
COMPILER_PATH = Path(os.environ.get("COMPILER_PATH", BASE_DIR / "bin" / "bangla_compiler"))
MAX_SOURCE_BYTES = int(os.environ.get("MAX_SOURCE_BYTES", "50000"))
COMPILE_TIMEOUT = float(os.environ.get("COMPILE_TIMEOUT", "5"))
RUN_TIMEOUT = float(os.environ.get("RUN_TIMEOUT", "2"))




def _decode_timeout_output(value):
    if value is None:
        return ""
    if isinstance(value, bytes):
        return value.decode("utf-8", errors="replace")
    return str(value)


def _run_process(command, cwd, timeout):
    return subprocess.run(
        command,
        cwd=cwd,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout,
        check=False,
        shell=False,
    )


def _read_text_if_exists(path):
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def compile_source(source, should_run):
    if not COMPILER_PATH.exists():
        return {
            "ok": False,
            "stage": "server",
            "message": f"Compiler binary not found at {COMPILER_PATH}",
        }

    with tempfile.TemporaryDirectory(prefix="suborno_") as temp_dir:
        workdir = Path(temp_dir)
        input_file = workdir / "input.bn"
        output_file = workdir / "output.py"
        input_file.write_text(source, encoding="utf-8")

        try:
            compiled = _run_process([str(COMPILER_PATH)], workdir, COMPILE_TIMEOUT)
        except subprocess.TimeoutExpired as exc:
            return {
                "ok": False,
                "stage": "compile",
                "message": "Compilation timed out.",
                "compiler_stdout": _decode_timeout_output(exc.stdout),
                "compiler_stderr": _decode_timeout_output(exc.stderr),
            }

        compiler_stdout = compiled.stdout
        compiler_stderr = compiled.stderr

        # These files are emitted directly by the compiler so the web UI can
        # show the real output of every compiler phase independently.
        stage_outputs = {
            "tokens": _read_text_if_exists(workdir / "tokens.txt"),
            "parse_tree": _read_text_if_exists(workdir / "parse_tree.txt"),
            "semantic": _read_text_if_exists(workdir / "semantic.txt"),
            "tac": _read_text_if_exists(workdir / "tac.txt"),
        }
        generated_python = _read_text_if_exists(output_file)

        base_response = {
            "compiler_stdout": compiler_stdout,
            "compiler_stderr": compiler_stderr,
            "generated_python": generated_python,
            **stage_outputs,
        }

        if compiled.returncode != 0 or not output_file.exists():
            return {
                "ok": False,
                "stage": "compile",
                "message": "Compilation failed.",
                "return_code": compiled.returncode,
                **base_response,
            }

        response = {
            "ok": True,
            "stage": "compiled",
            "message": "Compilation successful. Stage outputs are ready.",
            **base_response,
        }

        if not should_run:
            return response

        try:
            executed = _run_process([sys.executable, "output.py"], workdir, RUN_TIMEOUT)
        except subprocess.TimeoutExpired as exc:
            response.update(
                {
                    "ok": False,
                    "stage": "runtime",
                    "message": f"Program stopped after {RUN_TIMEOUT:g} seconds (possible infinite loop).",
                    "runtime_stdout": _decode_timeout_output(exc.stdout),
                    "runtime_stderr": _decode_timeout_output(exc.stderr),
                }
            )
            return response

        response.update(
            {
                "stage": "finished",
                "message": "Compilation and execution successful." if executed.returncode == 0 else "Program exited with an error.",
                "runtime_stdout": executed.stdout,
                "runtime_stderr": executed.stderr,
                "runtime_return_code": executed.returncode,
                "ok": executed.returncode == 0,
            }
        )
        return response


@app.get("/")
def home():
    example_source = _read_text_if_exists(BASE_DIR / "examples" / "integration.bn")
    return render_template("index.html", example_source=example_source)


@app.get("/health")
def health():
    return jsonify({"ok": True, "compiler": COMPILER_PATH.exists()})


@app.post("/api/compile")
def api_compile():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")
    should_run = bool(payload.get("run", False))

    if not isinstance(source, str) or not source.strip():
        return jsonify({"ok": False, "stage": "request", "message": "Source code is empty."}), 400

    if len(source.encode("utf-8")) > MAX_SOURCE_BYTES:
        return jsonify(
            {
                "ok": False,
                "stage": "request",
                "message": f"Source is too large. Maximum size is {MAX_SOURCE_BYTES} bytes.",
            }
        ), 413

    result = compile_source(source, should_run)
    status = 200 if result.get("stage") not in {"server"} else 500
    return jsonify(result), status


if __name__ == "__main__":
    port = int(os.environ.get("PORT", "10000"))
    app.run(host="0.0.0.0", port=port, debug=False)
