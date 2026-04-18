# EditorBMD

Editor desktop em C++/Win32 para abrir, visualizar, editar e salvar arquivos `.bmd` de itens do MU Online, respeitando a struct real usada pelo Main 5.2 - Takumi.

## Objetivo

O projeto foi feito para trabalhar com arquivos binarios de itens, carregando os registros em memoria, exibindo os campos em uma interface simples e permitindo edicao direta dos valores antes de salvar novamente no formato `.bmd`.

Nesta revisao, o editor foi alinhado com a struct `Script_Item` de `source/CGMItemMng.h`, removendo campos que nao existem no cliente e preservando o layout binario detectado no arquivo carregado.

## Stack

- C++
- Win32 API
- Common Controls (`ListView`, `ComboBox`)
- Visual Studio 2022
- Toolset `v143`
- Plataforma `x64`

## Estrutura

- `EditorBMD/Interface/EditorBMD.cpp`: janela principal, controles da UI, fluxo de abrir/salvar e edicao inline dos campos
- `EditorBMD/Interface/Main.h`: declaracoes globais, estado do arquivo e layout atual
- `EditorBMD/Interface/Resource.h`: IDs de recursos e comandos do menu
- `EditorBMD/Decrypt/BmdDecrypt.cpp`: leitura do `.bmd`, validacao de tamanho/checksum e conversao entre layouts
- `EditorBMD/Encrypt/BmdEncryption.cpp`: rotina de obfuscacao `BuxConvert` e geracao de checksum
- `EditorBMD/BMDCommon.h`: constantes, layouts suportados e definicao da struct `Script_Item`
- `docs/editorbmd-estrutura-item-main.md`: documentacao do alinhamento com a struct do cliente
- `docs/tema-dark-e-sobre.md`: documentacao da interface dark e da atualizacao da janela `Sobre`

## Como o editor funciona

1. O usuario abre um arquivo `.bmd` pelo menu `Arquivo > Abrir`.
2. O sistema le o cabecalho, valida o tamanho dos registros, detecta o layout e confere o checksum.
3. Os itens sao convertidos para `std::vector<Script_Item>`.
4. A tabela exibe todos os campos do item selecionado.
5. O valor de um campo pode ser editado diretamente na celula.
6. As alteracoes podem ser salvas em cima do mesmo arquivo ou com `Salvar Como`.
7. O layout detectado no arquivo aberto e preservado na gravacao.

## Interface atual

- Combo superior para filtro por section
- Busca por nome, section e index
- Tabela detalhada com todos os campos do `Item.bmd`
- Edicao inline por clique na celula
- Inclusao, duplicacao, copia e colagem de linhas
- Visual dark aplicado na janela principal e na janela `Sobre`
- Menu com:
  - `Abrir`
  - `Salvar`
  - `Salvar Como`
  - `Sair`
- Janela `Sobre` com:
  - Credito para `Vitor Araujo - VGaming`
  - Link do Discord
  - Link do repositorio

## Regras de leitura e escrita do BMD

- O arquivo comeca com `DWORD maxLine`
- Em seguida vem o bloco bruto com todos os `Script_Item`
- Ao final existe um `DWORD checksum`
- Cada item pode ocupar `155`, `157` ou `164` bytes, conforme o layout detectado
- O checksum e calculado com a chave `0xE2F1`
- Os bytes do bloco sao processados por `BuxConvert`

## Campos suportados

O editor expoe todos os campos descritos em `Script_Item`, incluindo:

- Identificacao: `Type`, `Index`, `SubIndex`, `Name`
- Combate: `DamageMin`, `DamageMax`, `Defense`, `MagicDefense`, `MagicPower`, `PowerATTK`
- Requisitos: `RequireStrength`, `RequireDexterity`, `RequireEnergy`, `RequireVitality`, `RequireCharisma`, `RequireLevel`
- Classe: `RequireClass[...]`
- Resistencias: `Resistance[...]`
- Flags diversas: `Drop`, `Trade`, `StorePersonal`, `WhareHouse`, `SellNpc`, `Expensive`, `Repair`, `Overlap`, `PcFlag`, `MuunFlag`

## Validacoes na edicao

Durante a edicao pela interface:

- `BYTE`: aceita valores de `0` a `255`
- `WORD`: aceita valores de `0` a `65535`
- `DWORD`: aceita valores inteiros maiores ou iguais a `0`
- `int`: aceita inteiros com sinal
- `string`: grava no buffer do campo

Quando um valor invalido e informado, o editor mostra uma mensagem de erro e nao aplica a alteracao.

## Changelog

- Ajustado o `editorBMD` para respeitar a struct real de `source/CGMItemMng.h`
- Removidos os campos inexistentes `Folder`, `Model` e `Padding`
- Adicionado suporte aos layouts de `155`, `157` e `164` bytes
- Preservacao automatica do layout detectado ao salvar o arquivo
- Grade da interface atualizada com os campos reais do cliente
- Interface atualizada para versao dark

## Compilacao

Requisitos:

- Visual Studio 2022
- Windows SDK compativel com o projeto
- Plataforma `x64`

Passos:

1. Abra `EditorBMD.sln`
2. Selecione a configuracao desejada (`Debug x64` ou `Release x64`)
3. Compile o projeto `EditorBMD`

## Limitacoes atuais

- O editor depende de um `Item.bmd` valido para preservar o layout original automaticamente
- Arquivos corrompidos ou com layout nao suportado continuam sendo rejeitados
- O projeto ainda pode evoluir com validacoes especificas por campo de gameplay
