global switch_context

section .text

switch_context:
    ; 1. Сохраняем регистры текущей задачи
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi

    ; Смещение аргументов: мы сделали 7 пушей (28 байт) + 4 байта (адрес возврата) = 32.
    ; Первый аргумент (old_esp) находится по адресу [esp + 32]
    mov eax, [esp + 32]
    mov [eax], esp

    ; Второй аргумент (new_esp) теперь находится по адресу [esp + 36]
    mov edx, [esp + 36]
    mov esp, edx

    ; 2. Восстанавливаем регистры новой задачи (симметрично пушам!)
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    ; 3. Теперь на вершине стека лежит чистый EIP (хоть для ядра, хоть для таски)
    ret
