# Monitor-and-test-Internet-connectivity

## Fluxo de funcionamento

```mermaid
flowchart TD

A[Inicialização do ESP32] --> B[Testa R1]
B --> C[Testa R2]
C --> D[Testa R3]

D --> E[Atualiza status dos links]

E --> F{Mudou o estado dos links?}

F -- Sim --> G[Ativa buzzer e sinaleira]
G --> H[Envia mensagem no Telegram]

F -- Não --> I[Aguarda intervalo]

H --> I

I --> J{Durante espera}

J --> K[Leitura botão ACK]
J --> L[Controle buzzer/sinaleira]
J --> M[Verifica comandos Telegram]

K --> N[Volta para novo ciclo]
L --> N
M --> N

N --> B
```

<p data-start="308" data-end="438">Sistema de monitoramento de múltiplas conexões de internet usando <strong data-start="374" data-end="391">ESP32 + W5500</strong>, com alarme local e notificações via Telegram.</p>
<h2 data-start="440" data-end="454">Visão geral</h2>
<p data-start="455" data-end="578">Este projeto monitora até <strong data-start="481" data-end="518">3 links de internet independentes</strong> (roteadores diferentes).<br data-start="543" data-end="546">
Quando um link falha, o sistema:</p>
<ul data-start="580" data-end="716">
<li data-start="580" data-end="606">
<p data-start="582" data-end="606">Ativa buzzer e sinaleira</p>
</li>
<li data-start="607" data-end="640">
<p data-start="609" data-end="640">Mostra o status no display OLED</p>
</li>
<li data-start="641" data-end="668">
<p data-start="643" data-end="668">Envia alerta via Telegram</p>
</li>
<li data-start="669" data-end="716">
<p data-start="671" data-end="716">Permite consulta remota com comando <code data-start="707" data-end="716">/status</code></p>
</li>
</ul>
<p data-start="718" data-end="812">Projetado para ambientes reais (lojas, oficinas, escritórios) onde quedas de fibra são comuns.</p>
<hr data-start="814" data-end="817">
<h2 data-start="819" data-end="837">Funcionalidades</h2>
<ul data-start="838" data-end="1142">
<li data-start="838" data-end="883">
<p data-start="840" data-end="883">Monitoramento sequencial de até <strong data-start="872" data-end="883">3 links</strong></p>
</li>
<li data-start="884" data-end="925">
<p data-start="886" data-end="925">Detecção de falha por teste TCP externo</p>
</li>
<li data-start="926" data-end="962">
<p data-start="928" data-end="962">Notificação automática no Telegram</p>
</li>
<li data-start="963" data-end="989">
<p data-start="965" data-end="989">Comando remoto <code data-start="980" data-end="989">/status</code></p>
</li>
<li data-start="990" data-end="1044">
<p data-start="992" data-end="1005">Alarme local:</p>
<ul data-start="1008" data-end="1044">
<li data-start="1008" data-end="1025">
<p data-start="1010" data-end="1025">buzzer pulsante</p>
</li>
<li data-start="1028" data-end="1044">
<p data-start="1030" data-end="1044">sinaleira fixa</p>
</li>
</ul>
</li>
<li data-start="1045" data-end="1082">
<p data-start="1047" data-end="1082">Botão <strong data-start="1053" data-end="1060">ACK</strong> para silenciar buzzer</p>
</li>
<li data-start="1083" data-end="1142">
<p data-start="1085" data-end="1142">Envio via <strong data-start="1095" data-end="1118">Ethernet disponível</strong> (sem depender de Wi-Fi)</p>
</li>
</ul>





<hr data-start="1144" data-end="1147">
<h2 data-start="1149" data-end="1170">Hardware utilizado</h2>
<ul data-start="1171" data-end="1391">
<li data-start="1171" data-end="1185">
<p data-start="1173" data-end="1185">ESP32 DevKit</p>
</li>
<li data-start="1186" data-end="1217">
<p data-start="1188" data-end="1217">3× módulos Ethernet <strong data-start="1208" data-end="1217">W5500</strong></p>
</li>
<li data-start="1218" data-end="1246">
<p data-start="1220" data-end="1246">Display OLED SSD1306 (I2C)</p>
</li>

<li data-start="1301" data-end="1326">
<p data-start="1303" data-end="1326">Módulo relé para a sinaleira</p>
</li>
<li data-start="1327" data-end="1339">
<p data-start="1329" data-end="1339">Módulo relé para o Buzzer 12V</p>
</li>
<li data-start="1340" data-end="1354">
<p data-start="1342" data-end="1354">Botão de ACK</p>
</li>
<li data-start="1355" data-end="1391">
<p data-start="1357" data-end="1391">Fonte 12V + step-down para 5V/3.3V</p>
</li>
</ul>
<hr data-start="1393" data-end="1396">
<h2 data-start="1398" data-end="1420">Ligações principais</h2>
<h3 data-start="1422" data-end="1445">SPI – módulos W5500</h3>
<div class="TyagGW_tableContainer"><div tabindex="-1" class="group TyagGW_tableWrapper flex flex-col-reverse w-fit"><table data-start="1446" data-end="1566" class="w-fit min-w-(--thread-content-width)"><thead data-start="1446" data-end="1463"><tr data-start="1446" data-end="1463"><th data-start="1446" data-end="1455" data-col-size="sm" class="">Função</th><th data-start="1455" data-end="1463" data-col-size="sm" class="">GPIO</th></tr></thead><tbody data-start="1482" data-end="1566"><tr data-start="1482" data-end="1494"><td data-start="1482" data-end="1488" data-col-size="sm">SCK</td><td data-start="1488" data-end="1494" data-col-size="sm">18</td></tr><tr data-start="1495" data-end="1508"><td data-start="1495" data-end="1502" data-col-size="sm">MISO</td><td data-start="1502" data-end="1508" data-col-size="sm">19</td></tr><tr data-start="1509" data-end="1522"><td data-start="1509" data-end="1516" data-col-size="sm">MOSI</td><td data-start="1516" data-end="1522" data-col-size="sm">23</td></tr><tr data-start="1523" data-end="1536"><td data-start="1523" data-end="1531" data-col-size="sm">CS R1</td><td data-start="1531" data-end="1536" data-col-size="sm">5</td></tr><tr data-start="1537" data-end="1551"><td data-start="1537" data-end="1545" data-col-size="sm">CS R2</td><td data-start="1545" data-end="1551" data-col-size="sm">17</td></tr><tr data-start="1552" data-end="1566"><td data-start="1552" data-end="1560" data-col-size="sm">CS R3</td><td data-start="1560" data-end="1566" data-col-size="sm">16</td></tr></tbody></table><div class="sticky h-0 select-none end-(--thread-content-margin) self-end"><div class="absolute end-0 flex items-end" style="height: 32.7969px;"><span class="" data-state="closed"><button aria-label="Copiar tabela" class="hover:bg-token-bg-tertiary text-token-text-secondary my-1 rounded-sm p-1 transition-opacity group-[:not(:hover):not(:focus-within)]:pointer-events-none group-[:not(:hover):not(:focus-within)]:opacity-0"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg></button></span></div></div></div></div>
<h3 data-start="1568" data-end="1590">I2C – display OLED</h3>
<div class="TyagGW_tableContainer"><div tabindex="-1" class="group TyagGW_tableWrapper flex flex-col-reverse w-fit"><table data-start="1591" data-end="1652" class="w-fit min-w-(--thread-content-width)"><thead data-start="1591" data-end="1608"><tr data-start="1591" data-end="1608"><th data-start="1591" data-end="1600" data-col-size="sm" class="">Função</th><th data-start="1600" data-end="1608" data-col-size="sm" class="">GPIO</th></tr></thead><tbody data-start="1627" data-end="1652"><tr data-start="1627" data-end="1639"><td data-start="1627" data-end="1633" data-col-size="sm">SDA</td><td data-start="1633" data-end="1639" data-col-size="sm">21</td></tr><tr data-start="1640" data-end="1652"><td data-start="1640" data-end="1646" data-col-size="sm">SCL</td><td data-start="1646" data-end="1652" data-col-size="sm">22</td></tr></tbody></table><div class="sticky h-0 select-none end-(--thread-content-margin) self-end"><div class="absolute end-0 flex items-end" style="height: 32.7969px;"><span class="" data-state="closed"><button aria-label="Copiar tabela" class="hover:bg-token-bg-tertiary text-token-text-secondary my-1 rounded-sm p-1 transition-opacity group-[:not(:hover):not(:focus-within)]:pointer-events-none group-[:not(:hover):not(:focus-within)]:opacity-0"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg></button></span></div></div></div></div>
<h3 data-start="1654" data-end="1672">Alarme e botão</h3>
<div class="TyagGW_tableContainer"><div tabindex="-1" class="group TyagGW_tableWrapper flex flex-col-reverse w-fit"><table data-start="1673" data-end="1762" class="w-fit min-w-(--thread-content-width)"><thead data-start="1673" data-end="1690"><tr data-start="1673" data-end="1690"><th data-start="1673" data-end="1682" data-col-size="sm" class="">Função</th><th data-start="1682" data-end="1690" data-col-size="sm" class="">GPIO</th></tr></thead><tbody data-start="1709" data-end="1762"><tr data-start="1709" data-end="1727"><td data-start="1709" data-end="1721" data-col-size="sm">Sinaleira</td><td data-start="1721" data-end="1727" data-col-size="sm">14</td></tr><tr data-start="1728" data-end="1743"><td data-start="1728" data-end="1737" data-col-size="sm">Buzzer</td><td data-start="1737" data-end="1743" data-col-size="sm">12</td></tr><tr data-start="1744" data-end="1762"><td data-start="1744" data-end="1756" data-col-size="sm">Botão ACK</td><td data-start="1756" data-end="1762" data-col-size="sm">27</td></tr></tbody></table><div class="sticky h-0 select-none end-(--thread-content-margin) self-end"><div class="absolute end-0 flex items-end" style="height: 32.7969px;"><span class="" data-state="closed"><button aria-label="Copiar tabela" class="hover:bg-token-bg-tertiary text-token-text-secondary my-1 rounded-sm p-1 transition-opacity group-[:not(:hover):not(:focus-within)]:pointer-events-none group-[:not(:hover):not(:focus-within)]:opacity-0"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg></button></span></div></div></div></div>
<hr data-start="1764" data-end="1767">
<h2 data-start="1769" data-end="1796">Funcionamento do sistema</h2>
<ol data-start="1797" data-end="2156">
<li data-start="1797" data-end="1837">
<p data-start="1800" data-end="1837">O ESP32 testa cada link em sequência.</p>
</li>
<li data-start="1838" data-end="1933">
<p data-start="1841" data-end="1859">Se um link falhar:</p>
<ul data-start="1863" data-end="1933">
<li data-start="1863" data-end="1877">
<p data-start="1865" data-end="1877">buzzer pulsa</p>
</li>
<li data-start="1881" data-end="1899">
<p data-start="1883" data-end="1899">sinaleira acende</p>
</li>
<li data-start="1903" data-end="1933">
<p data-start="1905" data-end="1933">mensagem enviada no Telegram</p>
</li>
</ul>
</li>
<li data-start="1934" data-end="1990">
<p data-start="1937" data-end="1958">Se outro link falhar:</p>
<ul data-start="1962" data-end="1990">
<li data-start="1962" data-end="1990">
<p data-start="1964" data-end="1990">nova notificação é enviada</p>
</li>
</ul>
</li>
<li data-start="1991" data-end="2084">
<p data-start="1994" data-end="2010">Se todos caírem:</p>
<ul data-start="2014" data-end="2084">
<li data-start="2014" data-end="2037">
<p data-start="2016" data-end="2037">alarme local continua</p>
</li>
<li data-start="2041" data-end="2084">
<p data-start="2043" data-end="2084">notificação é enviada quando algum voltar</p>
</li>
</ul>
</li>
<li data-start="2085" data-end="2156">
<p data-start="2088" data-end="2102">Botão <strong data-start="2094" data-end="2101">ACK</strong>:</p>
<ul data-start="2106" data-end="2156">
<li data-start="2106" data-end="2125">
<p data-start="2108" data-end="2125">silencia o buzzer</p>
</li>
<li data-start="2129" data-end="2156">
<p data-start="2131" data-end="2156">mantém a sinaleira ligada</p>
</li>
</ul>
</li>
</ol>
<hr data-start="2158" data-end="2161">
<h2 data-start="2163" data-end="2197">Exemplo de mensagem no Telegram</h2>
<pre class="overflow-visible! px-0!" data-start="2198" data-end="2270"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">yaml</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span><span class="hljs-string">🚨</span></span><span> </span><span><span class="hljs-attr">ALERTA:</span></span><span> </span><span><span class="hljs-string">Mudanca</span></span><span> </span><span><span class="hljs-literal">no</span></span><span> </span><span><span class="hljs-string">status</span></span><span> </span><span><span class="hljs-string">dos</span></span><span> </span><span><span class="hljs-string">links.</span></span><span>

</span><span><span class="hljs-attr">R1:</span></span><span> </span><span><span class="hljs-string">FALHA</span></span><span>
</span><span><span class="hljs-attr">R2:</span></span><span> </span><span><span class="hljs-string">OK</span></span><span>
</span><span><span class="hljs-attr">R3:</span></span><span> </span><span><span class="hljs-string">OK</span></span><span>
</span></span></code></div></div></pre>
<p data-start="2272" data-end="2287">Comando manual:</p>
<pre class="overflow-visible! px-0!" data-start="2289" data-end="2304"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">bash</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>/status
</span></span></code></div></div></pre>
<p data-start="2306" data-end="2315">Resposta:</p>
<pre class="overflow-visible! px-0!" data-start="2317" data-end="2373"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">yaml</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span><span class="hljs-attr">Status atual dos links:</span></span><span>

</span><span><span class="hljs-attr">R1:</span></span><span> </span><span><span class="hljs-string">OK</span></span><span>
</span><span><span class="hljs-attr">R2:</span></span><span> </span><span><span class="hljs-string">FALHA</span></span><span>
</span><span><span class="hljs-attr">R3:</span></span><span> </span><span><span class="hljs-string">OK</span></span><span>
</span></span></code></div></div></pre>
<hr data-start="2375" data-end="2378">
<h2 data-start="2380" data-end="2407">Configuração do Telegram</h2>
<h3 data-start="2409" data-end="2427">1) Criar o bot</h3>
<p data-start="2428" data-end="2446">No Telegram, abra:</p>
<pre class="overflow-visible! px-0!" data-start="2447" data-end="2465"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">css</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span><span class="hljs-keyword">@BotFather</span></span><span>
</span></span></code></div></div></pre>
<p data-start="2467" data-end="2473">Envie:</p>
<pre class="overflow-visible! px-0!" data-start="2474" data-end="2489"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">bash</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>/newbot
</span></span></code></div></div></pre>
<p data-start="2491" data-end="2514">Guarde o <strong data-start="2500" data-end="2513">BOT_TOKEN</strong>.</p>
<hr data-start="2516" data-end="2519">
<h3 data-start="2521" data-end="2547">2) Descobrir o CHAT_ID</h3>
<ol data-start="2548" data-end="2602">
<li data-start="2548" data-end="2580">
<p data-start="2551" data-end="2580">Envie uma mensagem para o bot</p>
</li>
<li data-start="2581" data-end="2602">
<p data-start="2584" data-end="2602">Abra no navegador:</p>
</li>
</ol>
<pre class="overflow-visible! px-0!" data-start="2604" data-end="2660"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">bash</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>https://api.telegram.org/botSEU_TOKEN/getUpdates
</span></span></code></div></div></pre>
<p data-start="2662" data-end="2670">Procure:</p>
<pre class="overflow-visible! px-0!" data-start="2671" data-end="2701"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">json</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span><span class="hljs-attr">"chat"</span></span><span><span class="hljs-punctuation">:</span></span><span><span class="hljs-punctuation">{</span></span><span><span class="hljs-attr">"id"</span></span><span><span class="hljs-punctuation">:</span></span><span><span class="hljs-number">123456789</span></span><span>
</span></span></code></div></div></pre>
<p data-start="2703" data-end="2731">Esse número é o <strong data-start="2719" data-end="2730">CHAT_ID</strong>.</p>
<hr data-start="2733" data-end="2736">
<h3 data-start="2738" data-end="2765">3) Configurar no código</h3>
<p data-start="2766" data-end="2778">No firmware:</p>
<pre class="overflow-visible! px-0!" data-start="2780" data-end="2852"><div class="contain-inline-size rounded-2xl corner-superellipse/1.1 relative bg-token-sidebar-surface-primary"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between h-9 bg-token-sidebar-surface-primary select-none rounded-t-2xl corner-t-superellipse/1.1">cpp</div><div class="sticky top-[calc(var(--sticky-padding-top)+9*var(--spacing))]"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"><button class="flex gap-1 items-center select-none py-1" aria-label="Copiar"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon-sm"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg>Copiar código</button></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre! language-cpp"><span><span><span class="hljs-meta">#<span class="hljs-keyword">define</span></span></span><span> BOT_TOKEN </span><span><span class="hljs-string">"SEU_TOKEN"</span></span><span>
</span><span><span class="hljs-meta">#<span class="hljs-keyword">define</span></span></span><span> CHAT_ID   </span><span><span class="hljs-string">"SEU_CHAT_ID"</span></span><span>
</span></span></code></div></div></pre>
<hr data-start="2854" data-end="2857">
<h2 data-start="2859" data-end="2877">Comandos do bot</h2>
<div class="TyagGW_tableContainer"><div tabindex="-1" class="group TyagGW_tableWrapper flex flex-col-reverse w-fit"><table data-start="2878" data-end="2965" class="w-fit min-w-(--thread-content-width)"><thead data-start="2878" data-end="2898"><tr data-start="2878" data-end="2898"><th data-start="2878" data-end="2888" data-col-size="sm" class="">Comando</th><th data-start="2888" data-end="2898" data-col-size="sm" class="">Função</th></tr></thead><tbody data-start="2918" data-end="2965"><tr data-start="2918" data-end="2965"><td data-start="2918" data-end="2930" data-col-size="sm"><code data-start="2920" data-end="2929">/status</code></td><td data-start="2930" data-end="2965" data-col-size="sm">Mostra o estado atual dos links</td></tr></tbody></table><div class="sticky h-0 select-none end-(--thread-content-margin) self-end"><div class="absolute end-0 flex items-end" style="height: 32.7969px;"><span class="" data-state="closed"><button aria-label="Copiar tabela" class="hover:bg-token-bg-tertiary text-token-text-secondary my-1 rounded-sm p-1 transition-opacity group-[:not(:hover):not(:focus-within)]:pointer-events-none group-[:not(:hover):not(:focus-within)]:opacity-0"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg></button></span></div></div></div></div>
<hr data-start="2967" data-end="2970">
<h2 data-start="2972" data-end="2997">Bibliotecas utilizadas</h2>
<p data-start="2998" data-end="3049">Instale pelo Gerenciador de Bibliotecas do Arduino:</p>
<ul data-start="3051" data-end="3123">
<li data-start="3051" data-end="3066">
<p data-start="3053" data-end="3066">EthernetESP32</p>
</li>
<li data-start="3067" data-end="3089">
<p data-start="3069" data-end="3089">UniversalTelegramBot</p>
</li>
<li data-start="3090" data-end="3108">
<p data-start="3092" data-end="3108">Adafruit SSD1306</p>
</li>
<li data-start="3109" data-end="3123">
<p data-start="3111" data-end="3123">Adafruit GFX</p>
</li>
</ul>

<hr data-start="3305" data-end="3308">
<h2 data-start="3310" data-end="3340">Possíveis melhorias futuras</h2>
<ul data-start="3341" data-end="3508">
<li data-start="3341" data-end="3375">
<p data-start="3343" data-end="3375">Registro de quedas com timestamp</p>
</li>
<li data-start="3376" data-end="3416">
<p data-start="3378" data-end="3416">Mensagens periódicas enquanto em falha</p>
</li>
<li data-start="3417" data-end="3452">
<p data-start="3419" data-end="3452">Comando <code data-start="3427" data-end="3439">/silenciar</code> via Telegram</p>
</li>
<li data-start="3453" data-end="3474">
<p data-start="3455" data-end="3474">Dashboard web local</p>
</li>
<li data-start="3475" data-end="3508">
<p data-start="3477" data-end="3508">RTC ou NTP para horário preciso</p>
</li>
</ul>
<hr data-start="3510" data-end="3513">
<h2 data-start="3515" data-end="3525">Licença</h2>
<p data-start="3526" data-end="3574">Projeto aberto para uso educacional e comercial.</p>
<hr data-start="3576" data-end="3579">
</ul>
</div>


https://github.com/Networking-for-Arduino/EthernetENC

https://github.com/dvarrel/ESPping
https://github.com/marian-craciunescu/ESP32Ping

<hr data-start="218" data-end="221">
<h1 data-start="223" data-end="249">Fontes principais usadas</h1>
<h2 data-start="251" data-end="293">1) Telegram Bot API (protocolo oficial)</h2>
<p data-start="294" data-end="358"><a data-start="294" data-end="356" rel="noopener" target="_new" class="decorated-link" href="https://core.telegram.org/bots/api">Telegram Bot API oficial<span aria-hidden="true" class="ms-0.5 inline-block align-middle leading-none"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" data-rtl-flip="" class="block h-[0.75em] w-[0.75em] stroke-current stroke-[0.75]"><use href="/cdn/assets/sprites-core-j9921kii.svg#304883" fill="currentColor"></use></svg></span></a></p>
<ul data-start="359" data-end="480">
<li data-start="359" data-end="388">
<p data-start="361" data-end="388">Documentação oficial da API</p>
</li>
<li data-start="389" data-end="480">
<p data-start="391" data-end="399">Explica:</p>
<ul data-start="402" data-end="480">
<li data-start="402" data-end="417">
<p data-start="404" data-end="417"><code data-start="404" data-end="417">sendMessage</code></p>
</li>
<li data-start="420" data-end="434">
<p data-start="422" data-end="434"><code data-start="422" data-end="434">getUpdates</code></p>
</li>
<li data-start="437" data-end="462">
<p data-start="439" data-end="462">estrutura das mensagens</p>
</li>
<li data-start="465" data-end="480">
<p data-start="467" data-end="480">uso via HTTPS</p>
</li>
</ul>
</li>
</ul>
<p data-start="482" data-end="500">Trecho importante:</p>
<blockquote data-start="501" data-end="640">
<p data-start="503" data-end="640">“The Bot API is an HTTP-based interface created for developers keen on building bots for Telegram.” <span class="" data-state="closed"><span class="ms-1 inline-flex max-w-full items-center select-none relative top-[-0.094rem] animate-[show_150ms_ease-in]" data-testid="webpage-citation-pill" style="width: 53px;"><a href="https://core.telegram.org/bots/api" target="_blank" rel="noopener" alt="https://core.telegram.org/bots/api" class="flex h-4.5 overflow-hidden rounded-xl px-2 text-[9px] font-medium transition-colors duration-150 ease-in-out text-token-text-secondary! bg-[#F4F4F4]! dark:bg-[#303030]!" style="max-width: 53px;"><span class="relative start-0 bottom-0 flex h-full w-full items-center"><span class="flex h-4 w-full items-center justify-between overflow-hidden" style="opacity: 1; transform: none;"><span class="max-w-[15ch] grow truncate overflow-hidden text-center">Telegram</span></span></span></a></span></span></p>
</blockquote>
<hr data-start="642" data-end="645">
<h2 data-start="647" data-end="706">2) Biblioteca UniversalTelegramBot (usada no seu código)</h2>
<p data-start="707" data-end="805"><a data-start="707" data-end="803" rel="noopener" target="_new" class="decorated-link" href="https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot">UniversalTelegramBot no GitHub<span aria-hidden="true" class="ms-0.5 inline-block align-middle leading-none"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" data-rtl-flip="" class="block h-[0.75em] w-[0.75em] stroke-current stroke-[0.75]"><use href="/cdn/assets/sprites-core-j9921kii.svg#304883" fill="currentColor"></use></svg></span></a></p>
<ul data-start="806" data-end="929">
<li data-start="806" data-end="833">
<p data-start="808" data-end="833">Biblioteca usada no ESP32</p>
</li>
<li data-start="834" data-end="893">
<p data-start="836" data-end="846">Base para:</p>
<ul data-start="849" data-end="893">
<li data-start="849" data-end="870">
<p data-start="851" data-end="870"><code data-start="851" data-end="870">bot.sendMessage()</code></p>
</li>
<li data-start="873" data-end="893">
<p data-start="875" data-end="893"><code data-start="875" data-end="893">bot.getUpdates()</code></p>
</li>
</ul>
</li>
<li data-start="894" data-end="929">
<p data-start="896" data-end="929">Exemplos de uso com ESP32 e HTTPS</p>
</li>
</ul>
<hr data-start="931" data-end="934">
<h2 data-start="936" data-end="977">3) Documentação do chip Ethernet W5500</h2>
<p data-start="978" data-end="1072"><a data-start="978" data-end="1070" rel="noopener" target="_new" class="decorated-link" href="https://docs.wiznet.io/Product/Chip/Ethernet/W5500">Documentação oficial do W5500 (Wiznet)<span aria-hidden="true" class="ms-0.5 inline-block align-middle leading-none"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" data-rtl-flip="" class="block h-[0.75em] w-[0.75em] stroke-current stroke-[0.75]"><use href="/cdn/assets/sprites-core-j9921kii.svg#304883" fill="currentColor"></use></svg></span></a></p>
<ul data-start="1073" data-end="1185">
<li data-start="1073" data-end="1113">
<p data-start="1075" data-end="1113">Manual técnico do controlador Ethernet</p>
</li>
<li data-start="1114" data-end="1185">
<p data-start="1116" data-end="1124">Explica:</p>
<ul data-start="1127" data-end="1185">
<li data-start="1127" data-end="1132">
<p data-start="1129" data-end="1132">SPI</p>
</li>
<li data-start="1135" data-end="1152">
<p data-start="1137" data-end="1152">sockets TCP/UDP</p>
</li>
<li data-start="1155" data-end="1185">
<p data-start="1157" data-end="1185">funcionamento de DHCP e rede</p>
</li>
</ul>
</li>
</ul>
<hr data-start="1187" data-end="1190">
<h2 data-start="1192" data-end="1259">4) GPIO e interrupções no ESP32 (documentação oficial Espressif)</h2>
<p data-start="1260" data-end="1394"><a data-start="1260" data-end="1392" rel="noopener" target="_new" class="decorated-link" href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html">ESP32 GPIO – documentação oficial<span aria-hidden="true" class="ms-0.5 inline-block align-middle leading-none"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" data-rtl-flip="" class="block h-[0.75em] w-[0.75em] stroke-current stroke-[0.75]"><use href="/cdn/assets/sprites-core-j9921kii.svg#304883" fill="currentColor"></use></svg></span></a></p>
<ul data-start="1395" data-end="1520">
<li data-start="1395" data-end="1428">
<p data-start="1397" data-end="1428">Guia oficial dos pinos do ESP32</p>
</li>
<li data-start="1429" data-end="1520">
<p data-start="1431" data-end="1441">Base para:</p>
<ul data-start="1444" data-end="1520">
<li data-start="1444" data-end="1468">
<p data-start="1446" data-end="1468">uso de GPIO27 no botão</p>
</li>
<li data-start="1471" data-end="1485">
<p data-start="1473" data-end="1485">interrupções</p>
</li>
<li data-start="1488" data-end="1520">
<p data-start="1490" data-end="1520">estados de boot (GPIO12, etc.)</p>
</li>
</ul>
</li>
</ul>
<hr data-start="1522" data-end="1525">
<h2 data-start="1527" data-end="1570">5) Referência de interrupções no Arduino</h2>
<p data-start="1571" data-end="1675">Link direto:<br>
<a data-start="1584" data-end="1675" rel="noopener" target="_new" class="decorated-link cursor-pointer">https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/<span aria-hidden="true" class="ms-0.5 inline-block align-middle leading-none"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" data-rtl-flip="" class="block h-[0.75em] w-[0.75em] stroke-current stroke-[0.75]"><use href="/cdn/assets/sprites-core-j9921kii.svg#304883" fill="currentColor"></use></svg></span></a></p>
<p data-start="1677" data-end="1687">Documenta:</p>
<ul data-start="1688" data-end="1752">
<li data-start="1688" data-end="1709">
<p data-start="1690" data-end="1709"><code data-start="1690" data-end="1709">attachInterrupt()</code></p>
</li>
<li data-start="1710" data-end="1752">
<p data-start="1712" data-end="1718">modos:</p>
<ul data-start="1721" data-end="1752">
<li data-start="1721" data-end="1729">
<p data-start="1723" data-end="1729">RISING</p>
</li>
<li data-start="1732" data-end="1741">
<p data-start="1734" data-end="1741">FALLING</p>
</li>
<li data-start="1744" data-end="1752">
<p data-start="1746" data-end="1752">CHANGE</p>
</li>
</ul>
</li>
</ul>
<p data-start="1754" data-end="1786">Base para o ACK por interrupção.</p>
<hr data-start="1788" data-end="1791">
<h1 data-start="1793" data-end="1843">Outras referências técnicas usadas (conceituais)</h1>
<p data-start="1844" data-end="1927">Essas não são de um único link específico, mas são práticas comuns documentadas em:</p>
<h3 data-start="1929" data-end="1949">Redes embarcadas</h3>
<ul data-start="1950" data-end="2044">
<li data-start="1950" data-end="1990">
<p data-start="1952" data-end="1990">Sequenciamento de múltiplas interfaces</p>
</li>
<li data-start="1991" data-end="2008">
<p data-start="1993" data-end="2008">Failover lógico</p>
</li>
<li data-start="2009" data-end="2044">
<p data-start="2011" data-end="2044">Testes TCP para detectar internet</p>
</li>
</ul>
<h3 data-start="2046" data-end="2079">Sistemas de alarme industrial</h3>
<ul data-start="2080" data-end="2151">
<li data-start="2080" data-end="2113">
<p data-start="2082" data-end="2113">ACK local com buzzer silenciado</p>
</li>
<li data-start="2114" data-end="2130">
<p data-start="2116" data-end="2130">Sinaleira fixa</p>
</li>
<li data-start="2131" data-end="2151">
<p data-start="2133" data-end="2151">Notificação remota</p>
</li>
</ul>
<hr data-start="2153" data-end="2156">
<h1 data-start="2158" data-end="2189">Resumo das tecnologias usadas</h1>
<div class="TyagGW_tableContainer"><div tabindex="-1" class="group TyagGW_tableWrapper flex flex-col-reverse w-fit"><table data-start="2190" data-end="2518" class="w-fit min-w-(--thread-content-width)"><thead data-start="2190" data-end="2225"><tr data-start="2190" data-end="2225"><th data-start="2190" data-end="2209" data-col-size="sm" class="">Parte do sistema</th><th data-start="2209" data-end="2225" data-col-size="md" class="">Base técnica</th></tr></thead><tbody data-start="2261" data-end="2518"><tr data-start="2261" data-end="2314"><td data-start="2261" data-end="2272" data-col-size="sm">Telegram</td><td data-start="2272" data-end="2314" data-col-size="md">Bot API oficial + UniversalTelegramBot</td></tr><tr data-start="2315" data-end="2366"><td data-start="2315" data-end="2326" data-col-size="sm">Ethernet</td><td data-start="2326" data-end="2366" data-col-size="md">W5500 datasheet + biblioteca Arduino</td></tr><tr data-start="2367" data-end="2409"><td data-start="2367" data-end="2375" data-col-size="sm">ESP32</td><td data-start="2375" data-end="2409" data-col-size="md">documentação oficial Espressif</td></tr><tr data-start="2410" data-end="2452"><td data-start="2410" data-end="2422" data-col-size="sm">Botão ACK</td><td data-start="2422" data-end="2452" data-col-size="md">interrupções Arduino/ESP32</td></tr><tr data-start="2453" data-end="2518"><td data-start="2453" data-end="2469" data-col-size="sm">Monitoramento</td><td data-start="2469" data-end="2518" data-col-size="md">lógica de failMask (técnica padrão de estado)</td></tr></tbody></table><div class="sticky h-0 select-none end-(--thread-content-margin) self-end"><div class="absolute end-0 flex items-end" style="height: 32.7969px;"><span class="" data-state="closed"><button aria-label="Copiar tabela" class="hover:bg-token-bg-tertiary text-token-text-secondary my-1 rounded-sm p-1 transition-opacity group-[:not(:hover):not(:focus-within)]:pointer-events-none group-[:not(:hover):not(:focus-within)]:opacity-0"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" aria-hidden="true" class="icon"><use href="/cdn/assets/sprites-core-j9921kii.svg#ce3544" fill="currentColor"></use></svg></button></span></div></div></div></div>
<hr data-start="2520" data-end="2523">
</ul>
</div>
