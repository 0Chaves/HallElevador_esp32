# HallElevador_esp32

## Visao geral

Este projeto representa o painel do hall do elevador. Ele trabalha em conjunto com o projeto `CabineElevador_esp32`, que representa a cabine/controlador do elevador.

A comunicacao entre os dois ESP32 acontece por MQTT:

- `HallElevador_esp32` envia chamadas de andar quando um botao externo e pressionado.
- `CabineElevador_esp32` recebe essas chamadas, move o elevador ate o andar solicitado e publica o estado atual da cabine.
- `HallElevador_esp32` recebe o estado publicado pela cabine e atualiza os LEDs do hall, indicando o andar atual e a chegada do elevador.

Ambos os projetos usam o mesmo broker MQTT:

| Configuracao | Valor |
| --- | --- |
| Broker | `192.168.1.114` |
| Porta | `1883` |
| Biblioteca MQTT | `PubSubClient` |
| Formato das mensagens | JSON |

## Fluxo de interacao

1. O usuario pressiona um botao no hall.
2. O `HallElevador_esp32` publica uma mensagem no topico `grupo5/hall/chamada_andar`.
3. O `CabineElevador_esp32`, inscrito nesse topico, recebe o andar solicitado.
4. A cabine marca esse andar como destino e executa o movimento do elevador.
5. Ao final do ciclo de movimento, a cabine publica seu estado no topico `grupo5/elevador/estado`.
6. O hall, inscrito nesse topico, atualiza os LEDs conforme o andar atual, o status da porta e a chegada do elevador.

## Topicos MQTT

| Projeto | Publica | Inscreve |
| --- | --- | --- |
| `HallElevador_esp32` | `grupo5/hall/chamada_andar` | `grupo5/elevador/estado` |
| `CabineElevador_esp32` | `grupo5/elevador/estado` | `grupo5/hall/chamada_andar` |

## Publicação do Hall para a Cabine

### Topico

`grupo5/hall/chamada_andar`

### Quando publica

O hall publica nesse topico sempre que um botao de andar e pressionado.

No codigo do hall, os botoes representam os andares de `0` a `3`:

| Valor | Andar |
| --- | --- |
| `0` | Terreo |
| `1` | Segundo andar |
| `2` | Terceiro andar |
| `3` | Quarto andar |

### Payload enviado

```json
{
  "andarDestino": 2
}
```

### Campos

| Campo | Tipo | Descricao |
| --- | --- | --- |
| `andarDestino` | inteiro | Andar chamado pelo usuario no painel do hall. Deve estar entre `0` e `3`. |

### O que a cabine faz ao receber

O `CabineElevador_esp32` esta inscrito em `grupo5/hall/chamada_andar`. Quando recebe a mensagem:

1. Le o campo `andarDestino`.
2. Verifica se o valor esta entre `0` e `ULTIMO_ANDAR`.
3. Marca esse andar como destino usando `elevador.setAndarDestino(andarDestino)`.
4. O metodo `moverElevador()` usa essa chamada para decidir se o elevador deve subir, descer ou permanecer parado.

## Publicacao da Cabine para o Hall

### Topico

`grupo5/elevador/estado`

### Quando publica

A cabine publica nesse topico ao final de cada ciclo do `loop()`, depois de verificar botoes internos, mover o elevador e atualizar seu estado.

### Payload enviado

```json
{
  "andar_atual": 2,
  "porta_status": 1,
  "elevador_chegada": true
}
```

### Campos

| Campo | Tipo | Descricao |
| --- | --- | --- |
| `andar_atual` | inteiro | Andar onde a cabine esta parada ou acabou de chegar. Usa valores de `0` a `3`. |
| `porta_status` | inteiro | Estado da porta lido pelo sensor magnetico. `0` significa fechada e `1` significa aberta. |
| `elevador_chegada` | booleano | Indica se o elevador chegou a um destino neste ciclo. `true` significa chegada confirmada; `false` significa que nao houve chegada. |

### O que o hall faz ao receber

O `HallElevador_esp32` esta inscrito em `grupo5/elevador/estado`. Quando recebe a mensagem:

1. Le `andar_atual`, `porta_status` e `elevador_chegada`.
2. Atualiza os atributos internos do objeto `Hall`.
3. Liga o LED correspondente ao andar atual da cabine.
4. Se `elevador_chegada` for `true` e `porta_status` for `1`, aciona o LED extra de chegada por alguns segundos.

## Responsabilidade de cada projeto

### `HallElevador_esp32`

- Le os botoes externos do hall.
- Publica chamadas de andar no MQTT.
- Recebe o estado da cabine.
- Atualiza os LEDs do hall com base no estado recebido.

### `CabineElevador_esp32`

- Le botoes internos da cabine.
- Recebe chamadas vindas do hall.
- Controla o movimento do elevador.
- Le o sensor magnetico da porta.
- Publica o estado atual da cabine no MQTT.

## Resumo

Assim, os dois projetos formam se complementam: o hall solicita destinos e a cabine devolve seu estado para que o hall consiga mostrar ao usuario onde o elevador esta e quando ele chegou.
