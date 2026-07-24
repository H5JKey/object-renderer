from core.exceptions.base import AuthenticationError, ForbiddenError


class InvalidPasswordError(AuthenticationError):
    """
    Ошибка, связанная с вводом неправильного пароля.
    """

    def __init__(self, password: str) -> None:
        self.password = password
        detail = f"Password {password} is invalid."
        super().__init__(detail)


class PermissionDeniedError(ForbiddenError):
    """
    Ошибка, связанная с отсутствием прав доступ.
    """

    def __init__(self, access_level: str) -> None:
        detail = f"Permission denied: access level = {access_level} is not sufficient."
        super().__init__(detail)
