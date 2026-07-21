from typing import Annotated

from fastapi import Depends
from repositories.file import FileRepository
from services.file_uploader import FileUploader

from dependencies.services import get_file_repository, get_input_file_uploader

FileRepositoryDep = Annotated[
    FileRepository,
    Depends(get_file_repository),
]

InputFileUploaderDep = Annotated[
    FileUploader,
    Depends(get_input_file_uploader),
]
