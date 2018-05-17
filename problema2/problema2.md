##Problema 2: Os Estudantes e o AE
O departamento de ciência da computação de uma universidade tem um
assistente de ensino (AE) que ajuda os estudantes de graduação em suas
tarefas de programação durante as horas normais de expediente. O
escritório do AE é pequeno e só tem espaço para uma mesa com uma
cadeira e um computador. Existem três cadeiras no corredor fora do
escritório em que os estudantes podem sentar e esperar se, no momento,
o AE estiver ajudando outro estudante. Quando não há estudantes que
precisem de ajuda durante as horas de expediente, o AE senta-se à mesa
e tira uma soneca. Se um estudante chega durante as horas de
expediente e encontra o AE dormindo, deve acordá-lo para pedir ajuda.
Se um estudante chega e encontra o AE ajudando outro estudante, deve
sentar-se em uma das cadeiras do corredor e esperar. Se não houver
cadeiras disponíveis, o estudante voltará mais tarde.
Usando threads, locks mutex e semáforos do POSIX, implemente uma 
solução que coordene as atividades do AE e os estudantes. Detalhes desse
exercício são fornecidos abaixo.

Usando o Pthreads, comece criando n estudantes (n aleatório, podendo
variar de 3 até 40). Cada estudante será executado como um thread
separado. O AE também será executado como um thread separado. Os
threads de estudantes se alternarão entre a programação por um período
de tempo aleatório e a busca de ajuda do AE. Se o AE estiver disponível,
eles obterão ajuda e, após receber ajuda 3 vezes, encerrarão sua
execução. Caso contrário, sentarão em uma cadeira no corredor (a
quantidade c de cadeiras deve ser igual a metade da quantidade n de
estudantes) ou, se não houver cadeiras disponíveis, voltarão a programar
e procurarão ajuda em um momento posterior. Se um estudante chegar
e notar que o AE está dormindo, deve notificar o AE usando um semáforo.
Quando o AE terminar de ajudar um estudante, deve verificar se há
estudantes esperando por ajuda no corredor. Se houver, o AE deve ajudar
esses estudantes, um de cada vez. Se não houver estudantes presentes,
o AE pode voltar a dormir. Talvez a melhor opção para a simulação da
programação dos estudantes — assim como do AE fornecendo ajuda a um
estudante — seja fazer os threads apropriados adormecerem por um
período de tempo aleatório.
Sempre que o estado de algum thread mudar, uma mensagem na tela
deve ser exibida indicando seu novo estado. Por exemplo, quando o AE
estiver atendendo o estudante 3, deve-se imprimir na tela “AE ajudando
o estudante 3”. Quando um estudante estiver sentado na fila, deve-se
imprimir “Estudante 3 sentado na cadeira” e assim por diante.

As **Seções 4.4.1 e 5.9.4 do livro em português** apresentam as
funções que devem ser utilizadas na resolução desse problema.