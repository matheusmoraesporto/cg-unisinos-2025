# Trabalho do grau B

Para compilação do projeto, como foi utilizado o Visual Studio Code, basta rodar `cmd + shift + B` para gerar o arquivo `app` na pasta raíz. Para rodar o projeto, basta executar este arquivo.

## Comandos para interação

Por padrão, a aplicação inicializa com os objetos em tela nas suas posições iniciais e o usuário pode interagir com a cena utilizando a camera. Os seguintes comandos permitem interagir com a cena:

- `C` para selecionar a camera
- `0` para selecionar o primeiro objeto (pokebola)
- `1` para selecionar o segundo objeto (esfera minecraft)
- Com a camera selecionada:
  - a movimentação do mouse, movimenta a camera
  - as teclas `A` e `D` movimentam a camera para os lados
  - tecla `W` aproxima a camera da cena
  - tecla `D` distancia a camera da cena
- Quando um dos objetos estiver selecionado:
  - `T` permite transladar o objeto utilizando as teclas `A`, `W`, `S`, `D`, `J` e `I`
  - `R` permite rotacionar o objeto utilizando as teclas `A`, `W`, `S`, `D`, `J` e `I`
  - `X` faz com que o objeto movimente-se em curva, conforme configurado no arquivo json
  - `[` diminui a escala do objeto (zoom out)
  - `]` aumenta a escala do objeto (zoom in)
- `esc` para encerrar a execução