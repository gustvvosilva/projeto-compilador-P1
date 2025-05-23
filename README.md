# Entrega P1: Projeto Completo

**Nome:** Gustavo Silva de Oliveira

*Repositório original do neander:* https://github.com/gustvvosilva/neander_proprio

*Repositório original do assembler:* https://github.com/gustvvosilva/assembler_proprio

## Como executar

- Compilação
  - Para compilar os códigos, escrevi um Makefile para facilitar o processo. Então basta apenas digitar **make** para compilar todos os códigos, já com todas flags que podem gerar erros ou warnings.
- Execução
  - Para executar o programa, digite **make run**. Isso vai compilar e executar o programa completo, onde o compilador recebe a linguagem, tranforma em assembly, envia ao assembler que gera o binário, e por fim o executor recebe o binário e executa, apresentando o resultado final e a memória, como é feito no Neander da UFRGS.

## Observações

O meu compilador ficou bem limitado, apenas consegui implementar a soma de positivos corretamente. Porém, o assembler e o executor ficaram bem completos, contando com soma, subtração e multiplicação de inteiros positivos e negativos. Para demonstrar todo o funcionamento desses dois juntos, deixei um código em assembly com nome de **exemplo02.asm**, que se trata de uma multiplicação de inteiros negativos, que mostra muito bem a capacidade do assembler, que eu infelizmente não consegui implementar no compilador.

- Para executar o **exemplo02.asm**, basta apenas digitar **make asm**, que ele irá usar esse assembly ao invés do gerado pelo compiler.
