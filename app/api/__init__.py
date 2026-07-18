__all__ = "router"
from fastapi import APIRouter

from .main_views import router as main_views_router

router = APIRouter(
    prefix="/api",
)
