##Problema 1: Validador de Sudoku

Um Sudoku utiliza um grid **9 x 9** no qual cada linha e cada coluna, bem
como cada um dos nove subgrids **3 x 3**, deve conter todos os dígitos 1 ...

Há diferentes formas de fazê-lo. Uma estratégia sugerida consiste em
criar threads que checam os seguintes critérios:
* Uma thread que checa se cada coluna contém os dígitos de 1 até 9
* Uma thread que checa se cada linha contém os dígitos de 1 até 9
* Nove threads que checam se cada subgrid de 3 x 3 contém os
dígitos de 1 até 9

Essa abordagem resultaria em onze threads distintas para validar o
Sudoku. Entretanto, você é livre para criar ainda mais threads for esse
projeto. Por exemplo, em vez de criar uma thread que checa todas as
nove colunas, poderiam ser criadas nove threads distintas e cada uma
checar uma coluna.

O grid deve ser lido a partir de um arquivo texto de entrada, contendo os
elementos do Sudoku. O resultado deve ser impresso na tela.

###Retornando resultados para a thread pai

Cada thread realizará a validação de determinada região do grid. Quando uma thread finaliza sua análise, é necessário retornar seu resultado para
a thread pai. Uma boa maneira de lidar com isso é criar um array de
inteiros visível por todas as threads. O i-ésimo índice no array
corresponde à i-ésima thread. Se uma thread escreve em sua respectiva
posição o valor 1, há indicação de que a região do Sudoku sobre sua
responsabilidade é válida. O valor 0 indica que não é válida. Quando todas
as threads terminarem suas tarefas, a thread pai realizará a checagem
de cada entrada do array para determinar se o grid apresentado é válido.