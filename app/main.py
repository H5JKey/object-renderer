from api import router as api_router
from api.main_views import router as main_router
from fastapi import FastAPI

app = FastAPI(
    title="Renderer",
)
app.include_router(main_router)
app.include_router(api_router)
