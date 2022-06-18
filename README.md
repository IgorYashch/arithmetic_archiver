# Мой архиватор, основанный на <code>[арифметическом кодировании](https://ru.wikipedia.org/wiki/%D0%90%D1%80%D0%B8%D1%84%D0%BC%D0%B5%D1%82%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%BE%D0%B5_%D0%BA%D0%BE%D0%B4%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5)</code>

### Требования:
> sudo apt update </br>
> sudo apt install -y cmake g++ python3

### Способ получить исполняемый файл в основной папке:
> mkdir build </br>
> cd build </br>
> cmake ../src </br>
> make && copy compress ../ compress </br>
> cd .. </br>


### Использование:
> ./compress</br>
>  --help                     = Напичатать информацию для использования</br>
>  --input  <file>            = Определить файл для сжатия/разархивации</br>
>  --output <file>            = Определить файл, получаемы после исполнения сжатия/разархивации</br>
>  --mode   {c | d}           = Специальные режимы, `c` для сжатия (compress) и `d` для разархивации (decompress)</br>
