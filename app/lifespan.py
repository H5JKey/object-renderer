import asyncio
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

from core.logging import configure_logging, get_logger
from fastapi import FastAPI
from infrastructure.kafka import (
    add_render_to_project_consume,
    add_render_to_project_consumer,
    generate_render_producer,
)

logger = get_logger(__name__)


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:  # noqa: ARG001
    """
    Действия до старта приложения.
    """
    configure_logging()
    await generate_render_producer.start()
    asyncio.create_task(  # noqa: RUF006
        add_render_to_project_consume(add_render_to_project_consumer),
    )
    logger.info("Application started")
    yield
    await generate_render_producer.stop()
    await add_render_to_project_consumer.stop()
    logger.info("Application has completed")
    """
    Действия при завершении работы приложения.
    """
