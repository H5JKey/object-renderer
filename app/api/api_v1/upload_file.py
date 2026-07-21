from dependencies.annotations import AuthUserByAccessTokenDep, InputFileUploaderDep
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
    user_id: AuthUserByAccessTokenDep,
) -> FileResponse:
    return await file_uploader.upload(
        user_id=user_id,
        file_name=uploaded_file.file,  # type: ignore[arg-type]
        file=uploaded_file.file,
    )
