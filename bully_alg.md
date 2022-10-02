Bully:
Quem iniciou a eleicao 
  [x] setar isOnElection pra true, pra evitar que inicie outra eleicao antes de terminar a atual
  [x] mandar o election packet pra todos acima dele
  Espera por answer packets
    se recebe nenhum em timeout de answer packet T, 
      [x] se elege manager (becomeManager)
      [x] envia manager packet pra todos
    Se recebe algum 
      inicia timeout de manage packet T’
        Se T’ acontece, reinicia election e seta isOnElection de todos pra false

Quem recebe um election packet
  [x] Responde answer packet de volta
    Se isOnElection == false
      [x] Inicia uma election

Quem recebe um manager packet
  Se id recebido é maior que o proprio
    atualiza a tabela
  Se id recebido é menor que o proprio
    Envia o proprio id como manager packet pra todos
    atualiza a tabela

Outras coisas:
  Quem ta acima dele:
    [x] um id pra cada máquina no map
    [x] inteiro contador incrementado a cada novo participante
    primeiro manager precisa se adicionar no map (id 1)
  [x] Adicionar flag isManager na classe machine
  [x] Definição de T e T’
    [x] lógica do timeout
    elected global
    isOnElection global


thread por election? pra isOnElection fazer sentido, ele precisa ser zerado. Pra zerar, precisamos reiniciar a thread, correto?

