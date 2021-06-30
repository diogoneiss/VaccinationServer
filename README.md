# Servidor de Mensagens

Execução: Individual

Data de entrega: 25 de junho de 2021

IntroduçãoProtocoloImplementaçãoAvaliação

Correção Semi-automáticaEntrega

Desconto de nota por atraso

# Introdução

Desde o início de 2020 o mundo vive um período de pandemia com o surgimento do vírus coronavírus (COVID-19) e suas variantes, o que mudou muitos dos aspectos das nossas vidas. Os cientistas do mundo têm trabalhado incansavelmente para termos vacinas e outras ações que nos permitam voltar à nossa antiga normalidade. Algumas vacinas surgiram e os países vivem diferentes momentos de vacinação onde pessoas de diferentes faixas etárias e proﬁssões estão vacinadas. Com o intuito de colaborar para que o cidadão saiba para onde se dirigir quando for o seu momento de vacinação, colocaremos em prática os conceitos aprendidos na disciplina de redes de computadores. Assim, neste trabalho iremos implementar um **sistema modelo servidor e clientes** para troca de mensagens tal que as pessoas possam achar um local de vacinação a partir da sua posição geográﬁca num dado momento.

O sistema possui 4 tipos de mensagens para comunicação clientes-servidor que foram divididos, para uma melhor compreensão, em dois grupos: SAÚDE e CIDADÃO. O primeiro grupo é composto por três tipos de mensagens que são usados para controle e conﬁguração do servidor pelas autoridades de saúde que indicarão os locais de vacinação. O último grupo é composto por apenas um tipo de mensagem que é usado para consulta e atendimento às solicitações dos clientes propriamente dito. **No trabalho, você deve implementar os quatro tipos de mensagens.**

# Protocolo

Com os tipos de mensagens do grupo SAÚDE, os clientes podemadicionar,remover e consultar locais de vacinação. Já com o único tipo de mensagem do grupoCIDADÃO,os clientes enviam para o servidor sua localização para conhecer o local mais próximo de vacinação. O servidor recebe as mensagens dos clientes, faz a veriﬁcação da localização atual e em seguida responde ao cliente qual é o local mais próximo. Um local de vacinação é deﬁnido através de uma posição geográﬁca (X, Y), com 0 ≤ X ≤ 9999 e 0 ≤ Y ≤9999.

O servidor e clientes trocam mensagens curtas de até 500 bytes usando o protocolo TCP. Mensagens carregam texto codiﬁcado segundo a tabela ASCII. Apenas letras, números e espaços podem ser transmitidos (caracteres acentuados não podem ser transmitidos).

Na sequência, são deﬁnidos o formato de cada tipo de mensagem, bem como a resposta esperada para cada uma deles.

### Grupo SAÚDE:

- **Adicionar local de vacinação:** clientes enviam uma mensagem no formato &quot; **add X Y**&quot; para oservidor.Oservidor,por sua vez, deve conﬁrmar a adição do local de vacinação com a mensagem &quot; **X Y added**&quot;. Caso o local **X Y** já tenha sido adicionado anteriormente, oservidordeveretornaramensagem&quot; **X**** Yalreadyexists**&quot;paraocliente.Serão

adicionados no máximo 50 locais de vacinação. Caso o limite seja atingido, o servidor deve retornar a mensagem &quot; **limit exceeded**&quot;.

- **Remover local de vacinação:** clientes enviam uma mensagem no formato &quot; **rm X Y**&quot; para oservidor.Oservidor,por sua vez, deve conﬁrmar que o local de vacinação foi removido com a mensagem &quot; **X Y removed**&quot;. Caso o local **X Y** não tenha sido adicionado anteriormente, o servidor deve retornar a mensagem &quot; **X Y does not**** exist**&quot;.
- **Consultar locais de vacinação:** clientes enviam uma mensagem no formato &quot; **list**&quot; para oservidor.Oservidor,por sua vez, retorna **todos** os locais de vacinação que foram adicionados para o cliente no formato &quot; **X**** 1 ****Y**** 1 ****X**** 2 ****Y**** 2 ****… X**** n ****Y**** n **&quot;. Caso nenhum local de vacinaçãotenhasidoadicionadoainda,oservidordeveretornaramensagem&quot;** none**&quot;.

### Grupo CIDADÃO:

- **Consultar o local de vacinação mais próximo:** clientes enviam uma mensagem contendo sua localização atual para o servidor no formato &quot; **query X Y**&quot;. Oservidor,por sua vez, deve retornar a coordenada do local de vacinação mais próximo ao local informado pelo cliente no formato &quot; **X Y**&quot;. Caso nenhum local de vacinação tenha sido adicionado ainda, o servidor deve retornar a mensagem&quot; **none**&quot;.

Abaixo seguem dois exemplos de comunicação do cliente com o servidor, onde linhas começando com \&gt; são enviadas pelo cliente e linhas começando com \&lt; foram recebidas do servidor:

### Exemplo 1

\&gt; add 111 111

\&lt; 111 111 added

\&gt; add 111 111

\&lt; 111 111 already exists

\&gt; add 222 222

\&lt; 222 222 added

- list

\&lt; 111 111 222 222

\&gt; rm 111 111

\&lt; 111 111 removed

\&gt; rm 111 111

\&lt; 111 111 does not exist

### Exemplo 2

\&gt; add 10 15

\&lt; 10 15 added

\&gt; add 89 100

\&lt; 89 100 added

- query 9090

\&lt; 89 100

- query 54

\&lt; 10 15

### Detalhes de implementação do protocolo:

  - As mensagens são terminadas com um caractere de quebra de linha&#39;\n&#39;. O caractere nulo&#39;\0&#39;para terminação de strings em C_não_deve ser enviado na rede.
  - O servidor deve descartar mensagens com caracteres inválidos ou com um formato desconhecido. O servidor pode desconectar o cliente que enviou a mensagem, mas precisa continuar a operação sem impacto para outrosclientes.
  - Para funcionamento do sistema de correção semi-automática (descrito abaixo), seu servidor deve fechar todas as conexões e terminar sua execução ao receber uma mensagem contendo apenas &quot; **kill**&quot; de qualquer um dosclientes.

Como especiﬁcado acima, mensagens podem ter até 500 bytes e o ﬁm de uma mensagem é identiﬁcado com um caractere&#39;\n&#39;. Uma mensagem não pode ultrapassar 500 bytes (i.e., um caractere&#39;\n&#39;deve aparecer entre os primeiros 500 bytes). Caso essas condições sejam violadas, o servidor pode inferir que há um_bug_no cliente e desconectá-lo.

Qualquer incoerência ou ambiguidade na especiﬁcação deve ser apontada para o professor.

# Implementação

O aluno deve implementar uma versão do servidor e uma versão do cliente. O servidor e o cliente devem utilizar o protocolo TCP, criado com [socket(AF\_INET, SOCK\_STREAM, 0)] ou com [socket(AF\_INET6, SOCK\_STREAM, 0)], para comunicação. Deve ser possível utilizar tanto IPv4 quanto IPv6.

Seu cliente deve receber mensagens do teclado e imprimir as mensagens recebidas na tela.

O servidor deve imprimir na saída padrão todas as mensagens recebidas de clientes. **Não é necessário** que seu servidor aceite mais de um cliente simultaneamente.

Seu servidor devereceber, **estritamente nessa ordem** , o tipo de endereço que será utilizado ( **v4** para IPv4 ou **v6** para IPv6) e um número de porta na linha de comando especiﬁcando em qual porta ele vai receber conexões. Seu cliente devereceber, **estritamente nessa ordem** , o endereço IP e a porta do servidor para estabelecimento da conexão. Exemplo de execução dos programas em dois terminaisdistintos:

### IPv4:

no terminal 1: ./servidor v4 51511

no terminal 2: ./cliente 127.0.0.1 51511

### IPv6:

no terminal 1: ./servidor v651511

no terminal 2: ./cliente ::151511

O servidor pode dar bind em todos os endereços IP associados às suas interfaces usando a constante INADDR\_ANY para IPv4 ou in6addr\_any para IPv6.

### Limites:

  - Cada mensagem possui no máximo 500bytes.
  - Serão adicionados no máximo 50 locais devacinação.
  - AscoordenadasXeYdevemservaloresinteirosentre0e9999(0≤X≤9999e0≤Y≤ 9999).

### Materiais para consulta:

- Capítulo2e3dolivrosobreprogramaçãocomsocketsdisponibilizadonoMoodle.

- [Playlist de programação comsoquetes](https://www.youtube.com/playlist?list=PLyrH0CFXIM5Wzmbv-lC-qvoBejsa803Qk).

# Avaliação

Este trabalho deve serrealizado individualmentee **deve ser implementado em C** utilizando apenas a biblioteca padrão (interface POSIX de soquetes de rede). Seu programa deve rodar no sistema operacional Linux e, em particular, **não deve utilizar bibliotecas do Windows,** como o winsock. Seu programa deve interoperar com qualquer outro programa implementando o mesmo protocolo (você pode testar com as implementações dos seus colegas). Procure escreverseucódigodemaneiraclara,comcomentáriospontuaisebemindentados.Istofacilita a correção dos monitores e tem impacto positivo naavaliação.

## CorreçãoSemi-automática

Seu servidor será corrigido de forma semi-automática por uma bateria de testes. Cada teste veriﬁca uma funcionalidade especíﬁca doservidor.O seu servidor será testado por um cliente implementado pelo professor com funcionalidades adicionais para realização dos testes. Os testes avaliam a aderência do seu servidor ao protocolo de comunicação inteiramente através dos dados trocados através da rede (a saída do seu servidor na tela, e.g., para depuração, não impacta os resultados dostestes).

O cliente implementado pelo professor para realização dos testes bem como um teste de exemplo estão disponíveis no Moodle para que você possa testar a compatibilidade de seu servidor com o ambiente de testes.

### Pelo menos os seguintes testes serão realizados:

- Adicionar locais devacinação.
- Remover locais devacinação.
- Listar todos os locais devacinação.
- Consultar o local de vacinação maispróximo.
- Recebimento de mensagens particionadas em múltiplas partes (mensagem recebida parcialmente no primeiro[recv]).
  - Estetestecobreocasodeumamensagemcom,porexemplo,20caracteres,que é enviada em dois pacotes. Neste teste, o servidor pode receber os primeiros 10 caracteres da mensagem em umrecve receber os últimos 10 caracteres da mensagem em umrecvsubsequente. É tarefa do servidor detectar que os primeiros 10 caracteres não possuem o&#39;\n&#39;, determinar que a mensagem ainda não chegou por completo, e chamarrecvnovamente até terminar de receber a mensagem para entãoprocessá-la.
- Recebimento de múltiplas mensagens em uma única chamada ao[recv].
  - Note que a sequência debytes

&quot;add 111 111 **\n**&quot;

contém uma mensagem, enquanto a sequência de bytes

&quot;add 111 111 **\n** add 222 222 **\n**&quot;

contém duas mensagens. Este teste cobre o segundo caso acima. Uma sequência de bytes, que pode ser lida em um únicorecv, contendo duas mensagens será enviada ao servidor. O servidor deve processar as duas mensagens corretamente.

Note que apesar do programa cliente não ser avaliado no conjunto de testes, ele ainda será avaliado manualmente pelo monitor.