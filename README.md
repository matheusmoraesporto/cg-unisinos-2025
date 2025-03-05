# Computação Gráfica 2025/1 - Unisinos

Este repositório é de uso único e exclusivo do aluno dono, para realização das tarefas da disciplina de Computação Gráfica.

## Setup MacOS

### Pré requisitos
1. Homebrew
2. VSCode
3. clang++

### Steps
Rodar o OpenGL com todas as bibliotecas necessárias, no MacOS, pode ser uma tarefa bem difícil no primeiro momento, uma vez que os códigos base desta disciplina estão voltados para o sistema Windows. Contudo não é tão complicado quanto parece fazer o setup no MacOS, aqui vão os passos:

**Instalar as dependências**

Para essa etapa devemos instalar todas as dependências necessárias via `homebrew`, para isso executaremos os seguintes comandos:
```
brew install glfw
```
```
brew install glm
```

Assim que estiverem instalados, é necessários extrair as dependências de cada uma dessas bibliotecas:
1. Para o `glfw`, acesse a pasta onde o brew instalou, no meu caso foi `/opt/homebrew/Cellar/glfw/3.4`, esse path pode mudar de acordo com a versão instalada.
2. Dentro da pasta correspondente ao `glfw`, deverá copie os arquivos de `include/GLFW` para o seu repositório, nas pastas `dependencies/include/GLFW`.
3. Em seguida volte para pasta onde está instalado o `glfw`, obtenha o arquivo `libglfw.3.4.dylib` da pasta `lib` e copie ele para a pasta `library` do seu repositório. **OBS: o nome do arquivo pode ser diferente de acordo com a versão.**
4. Faça o mesmo processo para o `glm`, obtenha o path onde ele foi instalado, no meu caso `/opt/homebrew/Cellar/glm`.
5. Dentro da pasta correspondente ao `glm`, deverá ter `<versão_do_glm>/include`, copie o conteúdo desta pasta e cole na seu repositório em `include/glm`.
6. Em seguida volte para pasta onde está instalado o `glm`, obtenha o arquivo `libglm.dylib` da pasta `lib` e copie ele para a pasta `library` do seu repositório.
7. Além disso é necessário obter o `glad`, para isso você deverá acessar este [link](https://glad.dav1d.de/):
    - 7.1. preencha o campo `gl` com a versão `3.3`
    - 7.2. preencha o campo `Profile` com `Core`
    - 7.3. clique no botão `Generate` para abrir a página de downloads e baixe o arquivo `glad.zip`
    - 7.4. abra o zip e copie as pastas `glad` e `KHR`, que estão dentro de `include` e cole ambas dentro da pasta `dependencies/include` do seu repositório
    - 7.5. por fim, acesse a pasta `src` extraída também do zip, copie e cole o arquivo `glad.c` para a pasta raíz do seu diretório.

## Como executar cada arquivo?
Utilizando a configuração predefinida no arquivo `tasks.json`, será necessário basicamente rodar o comando `Cmd + Shift + B`, dentro do VSCode e com o arquivo que deseja buildar aberto. Este comando de build irá gerar um arquivo executável com o nome `app` na pasta raíz do diretório, uma vez criado é necessário somente executar o `./app` para rodar a animação gerada.