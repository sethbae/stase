import urllib.request
from urllib.request import Request


def read_access_token() -> str:
    """
    Reads the lichess personal access token required to access stase's account.
    """
    with open(".env", "r") as file:
        token = file.read()

    return token.strip()


def register_account_as_bot(token: str) -> bool:
    """
    Sends a one-off request to register an account as a bot. Only needs to be done once per account.
    Returns True if the request was sent successfully and was accepted by lichess, False otherwise.
    """
    request = Request("https://lichess.org/api/bot/account/upgrade")
    request.add_header("Authorization", f"Bearer {token}")

    data = f"Authorization: Bearer {token}".encode("utf-8")

    response = urllib.request.urlopen(request, data=data)

    return "true" in str(response.read())
