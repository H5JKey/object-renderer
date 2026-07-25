from dependencies.annotations import RenderServiceDep
from fastapi import APIRouter, status
from schemas.render import RenderCreate

router = APIRouter(
    prefix="/render",
    tags=["Render"],
)


@router.post(
    "/create",
    status_code=status.HTTP_201_CREATED,
)
async def create_render(
    create_render_data: RenderCreate,
    render_service: RenderServiceDep,
) -> None:
    await render_service.create_render(create_render_data)
