from typing import Annotated

from fastapi import Depends
from schemas.auth import LoginRequest
from services.auth import AuthService
from services.file_uploader import FileUploader
from services.render import RenderService
from services.render_project import RenderProjectService
from services.user import UserService

from dependencies.auth import (
    get_auth_user_by_access_token,
    get_auth_user_by_refresh_token,
    get_login_request,
)
from dependencies.services import (
    get_auth_service,
    get_input_file_uploader,
    get_render_project_service,
    get_render_service,
    get_user_service,
)

InputFileUploaderDep = Annotated[
    FileUploader,
    Depends(get_input_file_uploader),
]

UserServiceDep = Annotated[
    UserService,
    Depends(get_user_service),
]

AuthServiceDep = Annotated[
    AuthService,
    Depends(get_auth_service),
]

AuthUserByAccessTokenDep = Annotated[
    int,
    Depends(get_auth_user_by_access_token),
]

AuthUserByRefreshTokenDep = Annotated[
    int,
    Depends(get_auth_user_by_refresh_token),
]

LoginRequestDep = Annotated[
    LoginRequest,
    Depends(get_login_request),
]


RenderServiceDep = Annotated[
    RenderService,
    Depends(get_render_service),
]

RenderProjectServiceDep = Annotated[
    RenderProjectService,
    Depends(get_render_project_service),
]
