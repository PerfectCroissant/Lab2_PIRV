## Задание
### Задача 1: Система интеллектуального контроля доступа
- Офисное здание с 5 входами и сенсорами доступа
- Каждое утро потоки сотрудников конкурируют за вход
- Система адаптируется: если поток людей возрастает, открываются дополнительные турникеты
- Высокоприоритетные сотрудники (директора, инженеры) получают приоритет
- Если турникет зависает или перестает работать, аварийная система автоматически перенаправляет людей
(threads.push_back(std::thread(employee, i, i % 5 == 0)); пример - Высокоприоритетные сотрудники с id 5, 10, 15, 20)
### Задача 2: Гибкая производственная линия
- 4 станка выполняют заказы с разными уровнями приоритета
- Если один станок выходит из строя, заказы автоматически перераспределяются
- Потоки срочных заказов могут прерывать обычные процессы
- Используется очередь с динамическими приоритетами для оптимальной загрузки
- Производственная линия автоматически адаптируется к нагрузке
(приоритет заказа, чем меньше число - тем выше приоритет)
