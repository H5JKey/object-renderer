from os import fstat
from typing import BinaryIO
from uuid import uuid4

from core.interfaces.clients import AbstractS3Client
from core.interfaces.repositories import AbstractFileRepository
from core.interfaces.services import AbstractFileUploader
from schemas.file import FileCreate, FileResponse


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

    async def upload(
        self,
        file_name: str,
        file: BinaryIO,
    ) -> FileResponse:
        key = self._generate_key(file_name)
        await self.s3_client.put_object(
            bucket=self.bucket,
            key=key,
            file=file,
        )
        file_descriptor = file.fileno()
        file_status = fstat(file_descriptor)
        size = file_status.st_size
        create_file_data = FileCreate(
            name=file.name,
            size=size,
            bucket=self.bucket,
            key=key,
        )

        created_file = await self.file_repository.create_file(
            create_file_data=create_file_data,
        )
        return FileResponse.model_validate(created_file)
