from collections.abc import AsyncGenerator
from typing import Annotated

from core.constants import s3_bucket
from fastapi import Depends
from infrastructure.database import session_factory
from infrastructure.minio.client import MinioClient
from repositories.file import FileRepository
from services.file_uploader import FileUploader
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
