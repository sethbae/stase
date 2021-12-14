import sys
from client import read_access_token, register_account_as_bot


def main():
    print(register_account_as_bot(read_access_token()))


if __name__ == "__main__":
    main()
    sys.exit(0)
