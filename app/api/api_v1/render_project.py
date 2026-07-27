from dependencies.annotations import AuthUserByAccessTokenDep, RenderProjectServiceDep
from fastapi import APIRouter
from schemas.render_project import RenderProjectResponse

router = APIRouter(
    prefix="/render-project",
    tags=["Render Project"],
)


@router.get("/{render_project_id}")
async def get_render_project(
    render_project_id: int,
    user_id: AuthUserByAccessTokenDep,
    render_project_service: RenderProjectServiceDep,
) -> RenderProjectResponse:
    return await render_project_service.get_by_id(
        render_project_id=render_project_id,
        user_id=user_id,
    )
