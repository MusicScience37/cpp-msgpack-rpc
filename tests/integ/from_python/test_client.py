"""Test to communicate from python client."""

# cspell: ignore packb

import asyncio
import pathlib

import msgpack
import pytest

HOST = "127.0.0.1"
PORT_NUMBER = 14876


class ClientProtocol(asyncio.Protocol):
    def __init__(self) -> None:
        self.sent_messages = asyncio.Queue()
        self.received_messages = asyncio.Queue()
        self._unpacker = msgpack.Unpacker()
        self._writer = None

    def connection_made(self, transport: asyncio.Transport) -> None:
        self._writer = asyncio.get_running_loop().create_task(
            self._write_data(transport)
        )

    async def _write_data(self, transport: asyncio.Transport) -> None:
        while True:
            message = await self.sent_messages.get()
            transport.write(message)

    def data_received(self, data: bytes) -> None:
        self._unpacker.feed(data)
        for message in self._unpacker:
            self.received_messages.put_nowait(message)

    def connection_lost(self, exc: Exception | None) -> None:
        if self._writer is not None:
            self._writer.cancel()


class ServerProcessProtocol(asyncio.SubprocessProtocol):
    def __init__(self) -> None:
        self.return_code_future = asyncio.get_running_loop().create_future()
        self._transport = None

    def connection_made(self, transport: asyncio.SubprocessTransport) -> None:
        self._transport = transport

    def process_exited(self) -> None:
        self.return_code_future.set_result(self._transport.get_returncode())


@pytest.mark.asyncio
async def test_echo(build_dir_path: pathlib.Path):
    loop = asyncio.get_running_loop()

    process, process_protocol = await loop.subprocess_exec(
        lambda: ServerProcessProtocol(),
        str(build_dir_path / "bin" / "integ_from_python_server"),
    )

    try:
        while True:
            try:
                transport, protocol = await loop.create_connection(
                    lambda: ClientProtocol(), host=HOST, port=PORT_NUMBER
                )
                break
            except:
                pass
        msgid = 12345
        data = "abc"
        protocol.sent_messages.put_nowait(msgpack.packb([0, msgid, "echo", [data]]))

        received_message = await protocol.received_messages.get()
        assert received_message == [1, msgid, None, data]

        transport.close()
    finally:
        process.terminate()
        await process_protocol.return_code_future
