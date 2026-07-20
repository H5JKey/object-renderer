from typing import Annotated

from fastapi import Depends

from dependencies.minio import get_model_uploader
from services.model_uploader import ModelUploader

ModelUploaderDep = Annotated[
    ModelUploader,
    Depends(get_model_uploader),
]
