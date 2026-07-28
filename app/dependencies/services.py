from collections.abc import AsyncGenerator
from typing import Annotated

from core.constants import s3_bucket
from fastapi import Depends
from infrastructure.database import session_factory
from infrastructure.kafka import ConfluentKafkaProducer
from infrastructure.minio.client import MinioClient
from repositories.file import FileRepository
from repositories.project import ProjectRepository
from repositories.user import UserRepository
from services.auth import AuthService
from services.file_uploader import FileUploader
from services.project import ProjectService
from services.render import RenderService
from services.user import UserService
from sqlalchemy.ext.asyncio import AsyncSession

from dependencies.minio import get_minio_client


async def get_session() -> AsyncGenerator[AsyncSession]:
    async with session_factory() as session:
        yield session


async def get_file_repository(
    session: Annotated[
        AsyncSession,
        Depends(get_session),
    ],
) -> AsyncGenerator[FileRepository]:
    file_repository = FileRepository(session)
    yield file_repository


async def get_input_file_uploader(
    s3_client: Annotated[
        MinioClient,
        Depends(get_minio_client),
    ],
    file_repository: Annotated[
        FileRepository,
        Depends(get_file_repository),
    ],
) -> AsyncGenerator[FileUploader]:
    file_uploader = FileUploader(
        bucket=s3_bucket.input_bucket,
        s3_client=s3_client,
        file_repository=file_repository,
    )
    yield file_uploader


async def get_user_repository(
    session: Annotated[
        AsyncSession,
        Depends(get_session),
    ],
) -> AsyncGenerator[UserRepository]:
    user_repository = UserRepository(session)
    yield user_repository


async def get_auth_service(
    user_repository: Annotated[
        UserRepository,
        Depends(get_user_repository),
    ],
) -> AsyncGenerator[AuthService]:
    auth_service = AuthService(user_repository)
    yield auth_service


async def get_user_service(
    user_repository: Annotated[
        UserRepository,
        Depends(get_user_repository),
    ],
) -> AsyncGenerator[UserService]:
    user_service = UserService(user_repository)
    yield user_service


def get_producer_config() -> dict:  # type: ignore[type-arg]
    config = {"bootstrap.servers": "kafka:9092"}
    return config


async def get_confluent_kafka_producer(
    config: Annotated[  # type: ignore[type-arg]
        dict,
        Depends(get_producer_config),
    ],
) -> AsyncGenerator[ConfluentKafkaProducer]:
    producer = ConfluentKafkaProducer(config)
    yield producer


async def get_render_service(
    producer: Annotated[
        ConfluentKafkaProducer,
        Depends(get_confluent_kafka_producer),
    ],
) -> AsyncGenerator[RenderService]:
    render_service = RenderService(producer)
    yield render_service


async def get_project_repository(
    session: Annotated[
        AsyncSession,
        Depends(get_session),
    ],
) -> AsyncGenerator[ProjectRepository]:
    project_repository = ProjectRepository(session)
    yield project_repository


async def get_project_service(
    project_repository: Annotated[
        ProjectRepository,
        Depends(get_project_repository),
    ],
) -> AsyncGenerator[ProjectService]:
    project_service = ProjectService(project_repository)
    yield project_service
