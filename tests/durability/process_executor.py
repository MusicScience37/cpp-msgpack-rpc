"""Class to execute processes."""

import datetime
import subprocess
import threading
import time
import typing

import pandas
import plotly.express
import psutil

DEFAULT_PROCESS_WAIT_TIME = 10.0


class ProcessExecutor:
    """Class to execute processes."""

    def __init__(
        self,
        cmd: list[str],
        cwd: str,
        log_prefix: str,
        *,
        write_plots: bool = True,
    ) -> None:
        self._process = subprocess.Popen(
            cmd,
            cwd=cwd,
            stdout=open(f"{cwd}/{log_prefix}_stdout.log", mode="w"),
            stderr=open(f"{cwd}/{log_prefix}_stderr.log", mode="w"),
            close_fds=True,
        )
        self._resources_prefix = f"{cwd}/{log_prefix}_resources"
        self._resources_file_path = f"{self._resources_prefix}.csv"
        self._watch_process_resources_thread = threading.Thread(
            target=self._watch_process_resources
        )
        self._watch_process_resources_thread.start()
        self._write_plots = write_plots

    def stop(self, timeout: float = DEFAULT_PROCESS_WAIT_TIME) -> None:
        if self._process.returncode is None:
            self._process.terminate()
            try:
                self._process.wait(timeout=timeout)
            except:
                self._process.kill()
        self._watch_process_resources_thread.join(timeout=1.0)
        if self._write_plots:
            self._plot_resources()

    def wait(self, timeout: float = DEFAULT_PROCESS_WAIT_TIME) -> None:
        self._process.wait(timeout=timeout)

    def __enter__(self) -> "ProcessExecutor":
        return self

    def __exit__(
        self, exc_type: typing.Any, exc_value: typing.Any, traceback: typing.Any
    ) -> None:
        self.stop()

    @property
    def returncode(self) -> int | None:
        return self._process.returncode

    def _watch_process_resources(self) -> None:
        with open(self._resources_file_path, mode="w") as file:
            file.write("Timestamp,Elapsed Time (sec),CPU (%),Memory (MB)\n")

            start_time = datetime.datetime.now()
            process = psutil.Process(self._process.pid)

            while process.is_running():
                timestamp = datetime.datetime.now()
                cpu_percent = process.cpu_percent()
                memory_mb = float(process.memory_info().rss) * 1e-6

                if not process.is_running():
                    return

                file.write(
                    f"{timestamp},{(timestamp - start_time).total_seconds()},"
                    f"{cpu_percent},{memory_mb}\n"
                )
                file.flush()

                time.sleep(0.1)

    def _plot_resources(self) -> None:
        data = pandas.read_csv(self._resources_file_path, parse_dates=["Timestamp"])

        figure = plotly.express.scatter(
            data,
            x="Elapsed Time (sec)",
            y="CPU (%)",
            trendline="ols",
        )
        figure.write_html(f"{self._resources_prefix}_cpu.html")
        figure.write_image(f"{self._resources_prefix}_cpu.png")

        figure = plotly.express.scatter(
            data,
            x="Elapsed Time (sec)",
            y="Memory (MB)",
            trendline="ols",
        )
        figure.write_html(f"{self._resources_prefix}_memory.html")
        figure.write_image(f"{self._resources_prefix}_memory.png")
