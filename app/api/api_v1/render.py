from fastapi import APIRouter, status
from schemas.render import RenderCreate, RenderResponse

router = APIRouter(
    prefix="/render",
    tags=["Render"],
)


@router.post(
    "/create",
    response_model=RenderResponse,
    status_code=status.HTTP_201_CREATED,
)
async def create_render(create_render_data: RenderCreate) -> RenderResponse:
    return RenderResponse(**create_render_data.model_dump())
