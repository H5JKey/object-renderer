import asyncio
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

from fastapi import FastAPI
from infrastructure.kafka import (
    add_render_to_project_consume,
    add_render_to_project_consumer,
    generate_render_producer,
)


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:  # noqa: ARG001
    """
    Действия до старта приложения.
    """
    await generate_render_producer.start()
    asyncio.create_task(  # noqa: RUF006
        add_render_to_project_consume(add_render_to_project_consumer),
    )
    yield
    await generate_render_producer.stop()
    await add_render_to_project_consumer.stop()
    """
    Действия при завершении работы приложения.
    """
