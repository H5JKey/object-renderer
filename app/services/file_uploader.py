import asyncio
from os import fstat
from typing import BinaryIO
from uuid import uuid4

from core.interfaces.clients import AbstractS3Client
from core.interfaces.repositories import AbstractFileRepository
from core.interfaces.services import AbstractFileUploader
from core.logging import get_logger
from schemas.file import FileCreate, FileResponse

logger = get_logger(__name__)


class FileUploader(AbstractFileUploader):
    def __init__(
        self,
        bucket: str,
        s3_client: AbstractS3Client,
        file_repository: AbstractFileRepository,
    ) -> None:
        self.s3_client = s3_client
        self.file_repository = file_repository
        self.bucket = bucket

    @staticmethod
    def _generate_key(file_name: str) -> str:
        uuid = uuid4()
        key = f"{uuid}_{file_name}"
        return key

    @staticmethod
    def _get_file_size(file: BinaryIO) -> int:
        """
        Получить размер файла в байтах.
        """
        file_descriptor = file.fileno()
        file_status = fstat(file_descriptor)
        size = file_status.st_size
        return size

    async def upload(
        self,
        file_name: str,
        file: BinaryIO,
    ) -> FileResponse:
        key = self._generate_key(file_name)
        size = self._get_file_size(file)
        create_file_data = FileCreate(
            name=file_name,
            size=size,
            bucket=self.bucket,
            key=key,
        )
        upload_file_to_s3_coroutine = self.s3_client.put_object(
            bucket=self.bucket,
            key=key,
            file=file,
        )
        create_file_coroutine = self.file_repository.create_file(
            create_file_data=create_file_data,
        )

        created_file, _ = await asyncio.gather(
            create_file_coroutine,
            upload_file_to_s3_coroutine,
        )

        logger.info(
            "Uploaded file, bucket=%s, key=%s, name=%s, size=%s",
            self.bucket,
            key,
            file_name,
            size,
        )
        return FileResponse.model_validate(created_file)
