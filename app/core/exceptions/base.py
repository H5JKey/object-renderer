class NotFoundError(Exception):
    """
    Ошибка, связанная с ненахождением объекта.
    """

    def __init__(self, detail: str) -> None:
        self.detail = detail
        super().__init__(detail)


class ConflictError(Exception):
    """
    Ошибка, связанная с конфликтами данных.
    """

    def __init__(self, detail: str) -> None:
        self.detail = detail
        super().__init__(detail)


class AuthenticationError(Exception):
    """
    Ошибка, связанная с аутентификацией.
    """

    def __init__(self, detail: str) -> None:
        self.detail = detail
        super().__init__(detail)


class ForbiddenError(Exception):
    """
    Ошибка, связанная с авторизацией.
    """

    def __init__(self, detail: str) -> None:
        self.detail = detail
        super().__init__(detail)
