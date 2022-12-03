#include <ESP8266WiFi.h> // Inclusão da biblioteca de funções wifi
#define botao1 0 // Define o pino GPIO00 como botão1
 
const char* ssid = "2G_Netvirtua_240/302";
const char* password = "0148657510";

WiFiServer server(80); //Shield irá receber as requisições das páginas (o padrão WEB é a porta 80)

String HTTP_req;
String URLValue;
int botao_estado = 1; 

//funções referentes à conexão wifi
String getURLRequest(String *requisicao);
bool mainPageRequest(String *requisicao);

//funções referentes à lógica do botão
void botao(byte porta); 
void botaoDigital(byte porta, WiFiClient cl);     

//funções referentes ao código HTML
void head(WiFiClient client);
void body(WiFiClient client);
void verde(WiFiClient client);
void azul(WiFiClient client);
void vermelha(WiFiClient client);
void amarela(WiFiClient client);

void setup()
{        
    Serial.begin(115200);
    pinMode(botao1, INPUT_PULLUP);
 
    //Conexão na rede WiFi
    delay(5000);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);
 
    WiFi.begin(ssid, password);
 
    //Espera a conexão ser estabelecida
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi conectado!");
 
    // Inicia o servidor WEB
    server.begin();
    Serial.println("Server iniciado");
 
    // Mostra o endereco IP
    Serial.println(WiFi.localIP());
}
 
void loop()
{
  WiFiClient  client = server.available();
  botao(botao1);
  
  if (client) { //Caso o cliente exista
    boolean currentLineIsBlank = true; //Cria variável booleana e atribui o valor verdadeiro
    while (client.connected()) { //Enquanto o cliente estiver conectado
      if (client.available()) { //Caso o cliente esteja disponível
        char c = client.read(); //Cria variável e atribui o valor de retorno do cliente
        HTTP_req += c;  //Soma à variável global do tipo String o novo caractere do cliente
        if (c == '\n' && currentLineIsBlank) { //Caso a variável do cliente seja uma linha em branco e a variável seja verdadeira

          if ( mainPageRequest(&HTTP_req) ) {
            URLValue = getURLRequest(&HTTP_req);
            Serial.println(HTTP_req);
                                      
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: keep-alive");
            client.println();
            
            //Conteudo da Página HTML
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            head(client);
            body(client);
            client.println("</html>");
            
          }else{
            Serial.println(HTTP_req);
            client.println("HTTP/1.1 200 OK");
          }
          HTTP_req = "";    
          break;
        }
        if (c == '\n') currentLineIsBlank = true;
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);    
    client.stop();
  }
}

String getURLRequest(String *requisicao) {
  int inicio, fim;
  String retorno;
 
  inicio = requisicao->indexOf("GET") + 3;
  fim = requisicao->indexOf("HTTP/") - 1;
  retorno = requisicao->substring(inicio, fim);
  retorno.trim();
 
  return retorno;
}
 
bool mainPageRequest(String *requisicao) {
  String valor;
  bool retorno = false;
 
  valor = getURLRequest(requisicao);
  valor.toLowerCase();
 
  if (valor == "/") retorno = true;
  if (valor.substring(0,2) == "/?") retorno = true; 
  if (valor.substring(0,10) == "/index.htm") retorno = true; 
 
  return retorno;
}

void botao(byte porta){
  if(digitalRead(botao1) == 0) botao_estado = 0;
  
  for (int i = 0; i<10; i++){
    if(digitalRead(porta) == 1) break;
    else{
      delay(1000);
      if(i > 8) {
        botao_estado = 1;
        delay(12000);
      }
    }
  } 
}

void botaoDigital(byte porta, WiFiClient cl){
  static boolean botao_status = 0;
  botao_status = digitalRead(porta);
  
  if(botao_status == 1){
    cl.println("<form method=\"get\">");
    cl.println("<a href=\"https://docs.google.com/forms/d/e/1FAIpQLSf_Q4YG_OJZa1JJdd99WJy9qo2iH7FBGOef3RAsUL_B0wOyow/viewform?usp=sf_link\">");
    cl.print("<input type=\"button\" name=\"bt\" value=\"Resolver\"");
    cl.print("onclick=\"value='Atualize a P&aacute;gina'\""); 
    cl.print(">");      
    cl.println("</a>");
    cl.println("</form>");       
  }else if(botao_status == 0){
    cl.println("ALERTA SISTEMA");
  }
}

void head(WiFiClient client){
  client.println("<head>");
  client.println("<title>IFPB - Banheiros</title>");
  client.println("<base target=\"_blank\">");
  if(botao_estado == 1) client.println("<meta http-equiv=\"refresh\" content=\"30\">");  
 
  client.println("<style>");
  client.println("#cabecalho {");
  client.println("text-align: center;");
  client.println("}");
  client.println(".extras {");
  client.println("text-decoration: none;");
  client.println("color: black;");
  client.println("background-color: gainsboro;");
  client.println("padding: 0px 10px;");
  client.println("}");
  client.println("#blocos {");
  client.println("text-align: center;");
  client.println("margin: 0 auto;");
  client.println("display: flex;");
  client.println("}");
  client.println(".bloco {");
  client.println("width: 300px;");
  client.println("padding: 5px 15px;");
  client.println("margin: 0 auto;");
  client.println("}");
  client.println("summary {");
  client.println("text-align: left;");
  client.println("padding: 10px 20px;");
  client.println("}");
  client.println("p {");
  client.println("text-align: justify;");
  client.println("font-size: small;");
  client.println("}");
  client.println("input {");
  client.println("margin: 0;");
  client.println("cursor: pointer;");  
  client.println("background-color: red;");
  client.println("}");
  client.println("</style>");
 
  client.println("</head>");
}

void body(WiFiClient client){
  client.println("<body>");
 
  client.println("<div id=\"cabecalho\">");
  client.println("<br/>");
  client.println("<h1>IFPB - Monitoramento dos banheiros do Campus Jo&atilde;o Pessoa.</h1>");
  client.println("<br/>");
  client.println("</div>");
 
  client.println("<div class=\"extras\">");
  client.println("<hr />");
  client.println("<a class=extras href=\"https://www.google.com\">Projeto</a>");
  client.println("<a class=extras href=\"https://docs.google.com/spreadsheets/d/1t3uA2vuHVD_cqOjmQXvUY8KYrE_zO5k9Ack2v8XNuoc/edit?usp=sharing\">Estat&iacute;sticas</a>");
  client.println("<hr />");
  client.println("</div>");
 
  client.println("<div id=\"blocos\">");
  verde(client);
  azul(client);
  vermelha(client);
  amarela(client);  
  client.println("</div>");              
 
  client.println("</body>");
}

void verde(WiFiClient client){
  client.println("<div class=\"bloco\">");
  client.println("<h2>&Aacute;rea Verde</h2>");
 
  client.println("<details>");
  client.println("<summary>Bloco de m&uacute;sica t&eacute;rreo</summary>");
  client.println("<p>O banheiro &eacute; localizado no principal corredor do bloco de m&uacute;sica, pr&oacute;ximo ao bebedouro, e tem acesso ao wifi.</p>");
  client.println("<p> Possui a divis&atilde;o sinalizada entre masculino e feminino, cada divis&atilde;o inclui v&aacute;rias cabines de sanit&aacute;rios e uma com chuveiro. Tamb&eacute;m possui dois ambientes pr&oacute;prios para deficientes de acordo com o sexo.</p>");
  client.println("</details>");
  if(botao_estado == 0) botaoDigital(botao1, client);
 
  client.println("<details>");
  client.println("<summary>Bloco de m&uacute;sica 1&deg; andar</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Gabinete m&eacute;dico</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Gin&aacute;sio 01</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Coordena&ccedil;&atilde;o educa&ccedil;&atilde;o f&iacutesica</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Funcion&aacute;rios coordena&ccedil;&atilde;o educa&ccedil;&atilde;o f&iacutesica</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Motoristas</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("</div>");
}
 
void azul(WiFiClient client){
  client.println("<div class=\"bloco\">");
  client.println("<h2>&Aacute;rea Azul</h2>");
 
  client.println("<details>");
  client.println("<summary>Bloco UAG t&eacute;rreo</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Bloco UAG 1&deg; andar</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
  
  client.println("<details>");
  client.println("<summary>Bloco UAG 2&deg; andar</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Bloco UAG 3&deg; andar</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Bloco Mec&acirc;nica</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Bloco Automa&ccedil;&atilde;o</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("</div>");
}

void vermelha(WiFiClient client){
  client.println("<div class=\"bloco\">");
  client.println("<h2>&Aacute;rea Vermelha</h2>");
 
  client.println("<details>");
  client.println("<summary>Bloco de ensino superior</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Salas de Aula 1 a 3</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Bloco de inform&aacute;tica</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Funcion&aacute;rios pr&oacute;ximo a copa</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Sala de estudos da biblioteca</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("</div>");  
}
 
void amarela(WiFiClient client){
  client.println("<div class=\"bloco\">");
  client.println("<h2>&Aacute;rea Amarela</h2>");
 
  client.println("<details>");
  client.println("<summary>Frente &agrave; biblioteca</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Bloco de ensino m&eacute;dio</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("<details>");
  client.println("<summary>Professores</summary>");
  client.println("<p>Descri&ccedil;&atilde;o Lorem ipsum dolor sit.</p>");
  client.println("</details>");
 
  client.println("</div>");
}