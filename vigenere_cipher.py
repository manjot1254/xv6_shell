class VigenereCipher:
    """
       VigenereCipher class

       Attributes:
           keyword (str): The word to be used as the key
           offset (int): The ASCII value of the letter 'A'. A static utility attribute to help you calculate
                a letter's position in the alphabet from 0 (A) to 25 (Z).
    """
    offset = ord('A')

    def __init__(self, keyword: str):
        self.keyword = keyword.upper()

    def __shift_letter(self, letter: str, shift: int, forward: bool):
        """
        Private method to shift a letter a number of positions forward or backward in the alphabet.

        Parameters
        ----------
        letter : str
            The letter to be shifted.
        shift: int
            The number to shift
        forward : bool
            Whether to shift forward (True) or backward (False).

        Returns
        -------
        str
            The shifted letter.
        """
        shifted_letter = ''
        P = ord(letter)
        C = P + shift

        if chr(P) == " ":
            shifted_letter = " "
        elif forward == True:
            shifted_letter = chr((C % 65)+65)
            if ord(shifted_letter) > 90:
                shifted_letter = chr(ord(shifted_letter)-26)
        else:
            shifted_letter = (chr(C-(2*shift)))
            if ord(shifted_letter) < 65:
                shifted_letter = chr(ord(shifted_letter)+26)

        return shifted_letter

    def __generate_shift_list(self, plaintext_length: int):
        """
        Private method to generate a list of integers. Each element is the shift to encrypt the
        plaintext letter at the same position.

        Vigenere cipher uses the keyword, repeated as many times as required to
        have the same number of letters as the plaintext, to derive the shift values for each
        plaintext letter.

        Parameters
        ----------
        plaintext_length : int
            The length of the plaintext after whitespace and punctuation has been removed.

        Returns
        -------
        list
            A list of shifts (integers).
        """
        shift_list = []
        counter = 0

        while counter < plaintext_length:
            for i in range(0, len(self.keyword)):
                shift_list.append(ord(self.keyword[i])%65)
                counter+=1
                if counter == plaintext_length:
                    return shift_list

    def encrypt(self, plaintext: str):
        """
        Encrypts a plaintext string.
        
        Parameters
        ----------
        plaintext : str
            The plaintext string.

        Returns
        -------
        str
            The ciphertext string.
        """
        ciphertext = ''
        plaintext = plaintext.upper().strip()

        temp = ""
        for i in range(0,len(plaintext)):
            if plaintext[i] == " " or plaintext[i] == "." or plaintext[i] == ",":
                continue
            else:
                temp = temp + plaintext[i]
        plaintext = temp
        
        shift_list = self.__generate_shift_list(len(plaintext))

        for i in range(0,len(plaintext)):
            ciphertext = ciphertext + self.__shift_letter(plaintext[i], shift_list[i], True)
        
        return ciphertext

    def decrypt(self, ciphertext: str):
        """
        Decrypts a ciphertext string
        
        Parameters
        ----------
        ciphertext : str
            The ciphertext string.

        Returns
        -------
        str
            The plaintext string.
        """
        plaintext = ''
        ciphertext = ciphertext.upper().strip()
        
        temp = ""
        for i in range(0,len(ciphertext)):
            if ciphertext[i] == " " or ciphertext[i] == "." or ciphertext[i] == ",":
                continue
            else:
                temp = temp + ciphertext[i]
        ciphertext = temp

        shift_list = self.__generate_shift_list(len(ciphertext))

        for i in range(0,len(ciphertext)):
            plaintext = plaintext + self.__shift_letter(ciphertext[i], shift_list[i], False)
        return plaintext


def main():
    # TESTS IN COMMENTS:
    # vig_cipher = VigenereCipher('smarties')
    # ciphertext = vig_cipher.encrypt("The quick brown fox jumps over the lazy dog")
    # plaintext = vig_cipher.decrypt(ciphertext)
    # assert plaintext == 'THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG'
    pass


if __name__ == "__main__":
    main()