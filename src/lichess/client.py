import json
import requests
from typing import Dict, List


API_BASE: str = "https://lichess.org/api"


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
    resp = requests.post(
        f"{API_BASE}/bot/account/upgrade",
        headers={"Authorization": f"Bearer {token}"}
    )

    return resp.status_code == 200


def get_incoming_challenges(token: str) -> List[Dict]:
    """
    Returns a list of incoming challenges for the given personal access token.
    Each challenge has quite a lot of info, for example:
    "id": ...
    "color": "random"
    "challenger": { ... }
    "rated": true
    etc
    """
    resp = requests.get(
        f"{API_BASE}/challenge",
        headers={"Authorization": f"Bearer {token}"}
    )

    return resp.json()["in"]


def respond_to_challenge(token: str, challenge_id: str, accept=True) -> bool:
    """
    Responds to a challenge with the given id. By default, accepts the challenge.
    Returns True if successful, and False if eg the challenge wasn't found.
    """
    resp_string = "accept" if accept else "decline"
    resp = requests.post(
        f"{API_BASE}/challenge/{challenge_id}/{resp_string}",
        headers={"Authorization": f"Bearer {token}"}
    )

    return resp.status_code == 200


def _open_stream(token: str, url: str):

    session = requests.Session()
    headers = {
        "Authorization": f"Bearer {token}",
        "connection": "keep-alive"
    }

    req = requests.Request("GET", url, headers=headers).prepare()
    resp = session.send(req, stream=True)

    for line in resp.iter_lines():
        if line:
            json_str = line.decode("utf-8").replace("'", '"')
            yield json.loads(json_str)


def stream_incoming_events(token: str):
    """
    Creates a stream of incoming events to the account. This is a generator, iterating over
    which will proceed only as new events arrive. Events include challenges, games starting etc.
    """
    url = f"{API_BASE}/stream/event"

    for event in _open_stream(token, url):
        yield event


def make_move(token: str, game_id: str, uci: str) -> bool:
    """
    Make a move in the given game, identified by the game id. Moves must be in UCI format,
    eg e2e4 or f7f8q if you need to promote.
    """
    r = requests.post(
        f"{API_BASE}/bot/game/{game_id}/move/{uci}",
        headers={"Authorization": f"Bearer {token}"}
    )

    return r.status_code == 200


def stream_game_events(token: str, game_id: str):
    """
    Creates a stream of incoming events in the given game. This is a generator, iterating over
    which will proceed only as new events arrive. Events include challenges, games starting etc.
    """
    url = f"{API_BASE}/bot/game/stream/{game_id}"

    for event in _open_stream(token, url):
        yield event


def resign_game(token: str, game_id: str) -> bool:
    """
    Resigns the game with the given id. Returns true if the request was successful
    and false otherwise.
    """
    r = requests.post(
        f"{API_BASE}/bot/game/{game_id}/resign",
        headers={"Authorization": f"Bearer {token}"}
    )

    return r.status_code == 200
