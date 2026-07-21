from typing import BinaryIO, cast

from aiobotocore.client import AioBaseClient
from core.interfaces.clients import AbstractS3Client


class MinioClient(AbstractS3Client):
    def __init__(self, s3_client: AioBaseClient) -> None:
        self.minio_client = s3_client

    async def generate_presigned_url(
        self,
        bucket: str,
        key: str,
        client_method: str,
        expires_in: int,
    ) -> str:
        params = {
            "Bucket": bucket,
            "Key": key,
        }
        presigned_url = await self.minio_client.generate_presigned_url(
            ClientMethod=client_method,
            Params=params,
            ExpiresIn=expires_in,
        )
        return cast(str, presigned_url)

    async def get_object(self, bucket: str, key: str) -> BinaryIO:
        file_response = await self.minio_client.get_object(
            Bucket=bucket,
            Key=key,
        )
        file_binary = file_response.read()
        return cast(BinaryIO, file_binary)

    async def put_object(
        self,
        bucket: str,
        key: str,
        file: BinaryIO,
    ) -> None:
        await self.minio_client.put_object(
            Bucket=bucket,
            Key=key,
            Body=file,
        )

    async def delete_object(self, bucket: str, key: str) -> None:
        await self.minio_client.delete_object(
            Bucket=bucket,
            Key=key,
        )
