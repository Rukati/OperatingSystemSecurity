## Реализация аутентификации внутри клинет-серверного приложения по схеме Лэмпорта.

## Описание
Протокол Лэмпорта основывается на хеш-функциях и предполагает, что как клиент, так и сервер располагают общим секретным ключом (каким-либо заранее установленным значением). При каждой аутентификации генерируется одноразовый пароль, который передается от клиента к серверу в зашифрованном виде.