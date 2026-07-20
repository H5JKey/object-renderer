from typing import BinaryIO

from core.interfaces import AbstractModelUploader, AbstractS3Client


class ModelUploader(AbstractModelUploader):
    def __init__(self, s3_client: AbstractS3Client) -> None:
        self.s3_client = s3_client

    async def upload_model(
        self,
        bucket: str,
        key: str,
        file: BinaryIO,
    ) -> None:
        await self.s3_client.put_object(
            bucket,
            key,
            file,
        )
