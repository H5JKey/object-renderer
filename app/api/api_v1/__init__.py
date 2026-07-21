__all__ = ("router",)

from fastapi import APIRouter

from .upload_file import router as upload_file_router

router = APIRouter(prefix="/v1")

router.include_router(upload_file_router)
