import ctypes

_ec_lib = None


def setup_binding() -> None:
    global _ec_lib
    _ec_lib = ctypes.CDLL("../../libengine_client.so")
    _ec_lib.get_computer_move.restype = ctypes.c_char_p
    _ec_lib.get_eval_str.restype = ctypes.c_char_p


class EngineClient:
    """
    The EngineClient wraps the C++ library in a standard Python object. Its interface is designed for playing
    a game with an opponent: it can either get a move from the current position, or update itself according
    to a move made by an opponent.
    """
    def __init__(self, fen: str = ""):
        if not _ec_lib:
            setup_binding()
        if fen:
            self._engine_client = _ec_lib.get_client(ctypes.c_char_p(bytes(fen)))
        else:
            self._engine_client = _ec_lib.get_client()

    def register_opponent_move(self, uci: str) -> None:
        """
        Update the engine so that it is aware of a move made by the opponent.
        """
        _ec_lib.register_opponent_move(self._engine_client, ctypes.c_char_p(uci.encode("utf8")))

    def get_computer_move(self, seconds: float) -> str:
        """
        Get the computer's move, with the given time to think.
        """
        return _ec_lib.get_computer_move(self._engine_client, ctypes.c_double(seconds)).decode("utf8")

    def get_node_count(self):
        """
        Fetches the number of nodes explored in the most recent move.
        """
        return _ec_lib.get_node_count(self._engine_client)

    def get_eval_str(self):
        """
        Fetches the computer evaluation for the most recent move.
        """
        return _ec_lib.get_eval_str(self._engine_client).decode("utf-8")
