import asyncio
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

from fastapi import FastAPI
from infrastructure.kafka import consume_messages, consumer, producer


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:  # noqa: ARG001
    """
    Действия до старта приложения.
    """
    await producer.start()

    asyncio.create_task(consume_messages(consumer))  # noqa: RUF006
    yield
    """
    Действия при завершении работы приложения.
    """
