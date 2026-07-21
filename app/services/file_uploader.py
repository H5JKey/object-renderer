from typing import BinaryIO
from uuid import uuid4

from core.interfaces import (
    AbstractFileRepository,
    AbstractFileUploader,
    AbstractS3Client,
)
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
        user_id: int,
        file_name: str,
        file: BinaryIO,
    ) -> FileResponse:
        key = self._generate_key(file_name)
        await self.s3_client.put_object(
            bucket=self.bucket,
            key=key,
            file=file,
        )
        create_file_data = FileCreate(
            bucket=self.bucket,
            key=key,
        )
        file = await self.file_repository.create_file(
            user_id=user_id,
            create_file_data=create_file_data,
        )
        return FileResponse.model_validate(file, from_attributes=True)
