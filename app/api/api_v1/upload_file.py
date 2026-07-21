from dependencies.annotations import InputFileUploaderDep
from fastapi import APIRouter, UploadFile, status
from schemas.file import FileResponse

router = APIRouter(
    tags=["Upload File"],
    prefix="/file",
)


@router.post(
    "/upload",
    status_code=status.HTTP_201_CREATED,
    response_model=FileResponse,
)
async def upload_file(
    file_uploader: InputFileUploaderDep,
    uploaded_file: UploadFile,
    user_id: int,
) -> FileResponse:
    return await file_uploader.upload(
        user_id,
        file=uploaded_file.file,
        file_name=uploaded_file.file,
    )
