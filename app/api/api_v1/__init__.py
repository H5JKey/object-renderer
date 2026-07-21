__all__ = ("router",)

from fastapi import APIRouter

from .auth import router as auth_router
from .upload_file import router as upload_file_router

router = APIRouter(
    prefix="/v1",
)

router.include_router(auth_router)
router.include_router(upload_file_router)
