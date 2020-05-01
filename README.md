Image_stegnography

This is a C programming project which encodes secret data into a .BMP image without affecting the properties of the image.

For Building : Type make



Usage : ./stegno [OPTION] [FILE DESCRIPTOR] [FILE NAME]...
[OPTION] < -e > : encode secret code to source image < -d > : decode secret code from stego image
For encoding : ./stegno -e -i <source image filename> -s <secret code filename > -o <stego image filename>
For decoding : ./stegno -d -i <stego image filename> -o <secret code filename>

