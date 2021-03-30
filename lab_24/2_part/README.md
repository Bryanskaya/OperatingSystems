## Полезные материалы

**Сам код**, полностью, а не как в материалах:  
http://www.interface.ru/home.asp?artId=17579  


### И самое интересное, проблема номер раз: 1001 версия ядра и не работающие функции
Поэтому смотрим тут, что можно использовать, а что нет  
https://elixir.bootlin.com/linux/v5.4/source/include/linux/fs.h  

## Как работает метод read()

![image](https://user-images.githubusercontent.com/54107546/112752947-32a33300-8fde-11eb-94c8-360494de9d90.png)  

#### Посмотреть наличие директории, файла и символической ссылки
```
ls -l /proc | grep my_
```

#### Взаимодействие с программой  
Запись в ядро  
```
echo "Hello" > /proc/my_fortune
```  

Чтение из ядра  
```
cat /proc/my_fortune
```

