### Требования:
sudo apt update
sudo apt install -y cmake g++ python3

### Способ получить исполняемый файл в основной папке:
mkdir build
cd build
cmake ../src
make && copy compress ../ compress
cd ..


### Использование:
./compress
  --help                     = Напичатать информацию для использования
  --input  <file>            = Определить файл для сжатия/разархивации
  --output <file>            = Определить файл, получаемы после исполнения сжатия/разархивации
  --mode   {c | d}           = Специальные режимы, `c` для сжатия (compress) и `d` для разархивации (decompress)
