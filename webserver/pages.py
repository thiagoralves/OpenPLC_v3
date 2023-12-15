login_head = """
<!DOCTYPE html>
<html>
    <style>
        @import url(https://fonts.googleapis.com/css?family=Roboto:300);
        .top {
            position:absolute;
            left:0; right:0; top:0;
            height: 50px;
            background-color: #000000;
            position: fixed;
            overflow: hidden;
            z-index: 10
        }
       .main {
            position: absolute;
            left:0px; top:50px; right:0; bottom:0;
        }
        .login-page {
          width: 360px;
          padding: 4% 0 0;
          margin: auto;
        }
        .form {
          position: relative;
          z-index: 1;
          background: #FFFFFF;
          max-width: 360px;
          margin: 0 auto 10px;
          padding: 45px;
          text-align: center;
          box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24);
        }
        .form input {
          font-family: 'Roboto', sans-serif;
          outline: 0;
          background: #f2f2f2;
          width: 100%;
          border: 0;
          margin: 0 0 15px;
          padding: 15px;
          box-sizing: border-box;
          font-size: 14px;
        }
        .form button {
          font-family: 'Roboto', sans-serif;
          text-transform: uppercase;
          outline: 0;
          background: #0066fc;
          width: 100%;
          border: 0;
          padding: 15px;
          color: #FFFFFF;
          font-size: 14px;
          -webkit-transition: all 0.3 ease;
          transition: all 0.3 ease;
          cursor: pointer;
        }
        .form button:hover,.form button:active,.form button:focus {
          background: #00337e;
        }
        .form .message {
          margin: 15px 0 0;
          color: #b3b3b3;
          font-size: 12px;
        }
        .form .message a {
          color: #4CAF50;
          text-decoration: none;
        }
        .form .register-form {
          display: none;
        }
        .container {
          position: relative;
          z-index: 1;
          max-width: 300px;
          margin: 0 auto;
        }
        .container:before, .container:after {
          content: '';
          display: block;
          clear: both;
        }
        .container .info {
          margin: 50px auto;
          text-align: center;
        }
        .container .info h1 {
          margin: 0 0 15px;
          padding: 0;
          font-size: 36px;
          font-weight: 300;
          color: #1a1a1a;
        }
        .container .info span {
          color: #4d4d4d;
          font-size: 12px;
        }
        .container .info span a {
          color: #000000;
          text-decoration: none;
        }
        .container .info span .fa {
          color: #EF3B3A;
        }
        body {
          background: #76b852; /* fallback for old browsers */
          background: -webkit-linear-gradient(right, #626262, #3D3D3D);
          background: -moz-linear-gradient(right, #626262, #3D3D3D);
          background: -o-linear-gradient(right, #626262, #3D3D3D);
          background: linear-gradient(to left, #626262, #3D3D3D);
          font-family: 'Roboto', sans-serif;
          -webkit-font-smoothing: antialiased;
          -moz-osx-font-smoothing: grayscale;
        }
    </style>
    <body>
        <div class='top'>
            <img src="/static/logo-openplc.png" alt="OpenPLC" style="width:63px;height:50px;padding:0px 0px 0px 10px;float:left">
            <h3 style=\"font-family:'Roboto', sans-serif; font-size:18px; color:white; padding:13px 111px 0px 0px; margin: 0px 0px 0px 0px\"><center>OpenPLC Webserver</center></h3>
        </div>
        <div class='main'>
            <div class='login-page'>
              <div class='form'>
                <form action='login' method='POST' class='login-form'>
                    <h3 style=\"font-family:'Roboto', sans-serif; font-size:24px; color:#1F1F1F; padding:0px 0px 0px 0px; margin: 0px 0px 40px 0px\"><center><b>Welcome to OpenPLC</b></center></h3>"""

login_body = """
                    <h3 style=\"font-family:'Roboto', sans-serif; font-size:14px; color:#1F1F1F; padding:0px 0px 0px 0px; margin: 0px 0px 40px 0px\"><center>Use your credentials to login</center></h3>
                    <input type='text' name='username' id='username' placeholder='username'/>
                    <input type='password' name='password' id='password' placeholder='password'/>
                    <br><br><br>
                    <button>login</button>
                </form>
              </div>
            </div>
        </div>
    </body>
</html>"""

bad_login_body = """
                    <h3 style=\"font-family:'Roboto', sans-serif; font-size:14px; color:red; font-weight: bold; padding:0px 0px 0px 0px; margin: 0px 0px 40px 0px\"><center>Bad credentials! Try again</center></h3>
                    <input type='text' name='username' id='username' placeholder='username'/>
                    <input type='password' name='password' id='password' placeholder='password'/>
                    <br><br><br>
                    <button>login</button>
                </form>
              </div>
            </div>
        </div>
    </body>
</html>"""

w3_style = """
<!DOCTYPE html>
<html>
    <style>
        /* W3.CSS 4.09 January 2018 by Jan Egil and Borge Refsnes */
        html{box-sizing:border-box}*,*:before,*:after{box-sizing:inherit}
        /* Extract from normalize.css by Nicolas Gallagher and Jonathan Neal git.io/normalize */
        html{-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}body{margin:0}
        article,aside,details,figcaption,figure,footer,header,main,menu,nav,section,summary{display:block}
        audio,canvas,progress,video{display:inline-block}progress{vertical-align:baseline}
        audio:not([controls]){display:none;height:0}[hidden],template{display:none}
        a{background-color:transparent;-webkit-text-decoration-skip:objects}
        a:active,a:hover{outline-width:0}abbr[title]{border-bottom:none;text-decoration:underline;text-decoration:underline dotted}
        dfn{font-style:italic}mark{background:#ff0;color:#000}
        small{font-size:80%}sub,sup{font-size:75%;line-height:0;position:relative;vertical-align:baseline}
        sub{bottom:-0.25em}sup{top:-0.5em}figure{margin:1em 40px}img{border-style:none}svg:not(:root){overflow:hidden}
        code,kbd,pre,samp{font-family:monospace,monospace;font-size:1em}hr{box-sizing:content-box;height:0;overflow:visible}
        button,input,select,textarea{font:inherit;margin:0}optgroup{font-weight:bold}
        button,input{overflow:visible}button,select{text-transform:none}
        button,html [type=button],[type=reset],[type=submit]{-webkit-appearance:button}
        button::-moz-focus-inner, [type=button]::-moz-focus-inner, [type=reset]::-moz-focus-inner, [type=submit]::-moz-focus-inner{border-style:none;padding:0}
        button:-moz-focusring, [type=button]:-moz-focusring, [type=reset]:-moz-focusring, [type=submit]:-moz-focusring{outline:1px dotted ButtonText}
        fieldset{border:1px solid #c0c0c0;margin:0 2px;padding:.35em .625em .75em}
        legend{color:inherit;display:table;max-width:100%;padding:0;white-space:normal}textarea{overflow:auto}
        [type=checkbox],[type=radio]{padding:0}
        [type=number]::-webkit-inner-spin-button,[type=number]::-webkit-outer-spin-button{height:auto}
        [type=search]{-webkit-appearance:textfield;outline-offset:-2px}
        [type=search]::-webkit-search-cancel-button,[type=search]::-webkit-search-decoration{-webkit-appearance:none}
        ::-webkit-input-placeholder{color:inherit;opacity:0.54}
        ::-webkit-file-upload-button{-webkit-appearance:button;font:inherit}
        /* End extract */
        html,body{font-family:Verdana,sans-serif;font-size:15px;line-height:1.5}html{overflow-x:hidden}
        h1{font-size:36px}h2{font-size:30px}h3{font-size:24px}h4{font-size:20px}h5{font-size:18px}h6{font-size:16px}.w3-serif{font-family:serif}
        h1,h2,h3,h4,h5,h6{font-family:'Segoe UI',Arial,sans-serif;font-weight:400;margin:10px 0}.w3-wide{letter-spacing:4px}
        hr{border:0;border-top:1px solid #eee;margin:20px 0}
        .w3-image{max-width:100%;height:auto}img{vertical-align:middle}a{color:inherit}
        .w3-table,.w3-table-all{border-collapse:collapse;border-spacing:0;width:100%;display:table}.w3-table-all{border:1px solid #ccc}
        .w3-bordered tr,.w3-table-all tr{border-bottom:1px solid #ddd}.w3-striped tbody tr:nth-child(even){background-color:#f1f1f1}
        .w3-table-all tr:nth-child(odd){background-color:#fff}.w3-table-all tr:nth-child(even){background-color:#f1f1f1}
        .w3-hoverable tbody tr:hover,.w3-ul.w3-hoverable li:hover{background-color:#ccc}.w3-centered tr th,.w3-centered tr td{text-align:center}
        .w3-table td,.w3-table th,.w3-table-all td,.w3-table-all th{padding:8px 8px;display:table-cell;text-align:left;vertical-align:top}
        .w3-table th:first-child,.w3-table td:first-child,.w3-table-all th:first-child,.w3-table-all td:first-child{padding-left:16px}
        .w3-btn,.w3-button{border:none;display:inline-block;padding:8px 16px;vertical-align:middle;overflow:hidden;text-decoration:none;color:inherit;background-color:inherit;text-align:center;cursor:pointer;white-space:nowrap}
        .w3-btn:hover{box-shadow:0 8px 16px 0 rgba(0,0,0,0.2),0 6px 20px 0 rgba(0,0,0,0.19)}
        .w3-btn,.w3-button{-webkit-touch-callout:none;-webkit-user-select:none;-khtml-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}   
        .w3-disabled,.w3-btn:disabled,.w3-button:disabled{cursor:not-allowed;opacity:0.3}.w3-disabled *,:disabled *{pointer-events:none}
        .w3-btn.w3-disabled:hover,.w3-btn:disabled:hover{box-shadow:none}
        .w3-badge,.w3-tag{background-color:#000;color:#fff;display:inline-block;padding-left:8px;padding-right:8px;text-align:center}.w3-badge{border-radius:50%}
        .w3-ul{list-style-type:none;padding:0;margin:0}.w3-ul li{padding:8px 16px;border-bottom:1px solid #ddd}.w3-ul li:last-child{border-bottom:none}
        .w3-tooltip,.w3-display-container{position:relative}.w3-tooltip .w3-text{display:none}.w3-tooltip:hover .w3-text{display:inline-block}
        .w3-ripple:active{opacity:0.5}.w3-ripple{transition:opacity 0s}
        .w3-input{padding:8px;display:block;border:none;border-bottom:1px solid #ccc;width:100%}
        .w3-select{padding:9px 0;width:100%;border:none;border-bottom:1px solid #ccc}
        .w3-dropdown-click,.w3-dropdown-hover{position:relative;display:inline-block;cursor:pointer}
        .w3-dropdown-hover:hover .w3-dropdown-content{display:block}
        .w3-dropdown-hover:first-child,.w3-dropdown-click:hover{background-color:#ccc;color:#000}
        .w3-dropdown-hover:hover > .w3-button:first-child,.w3-dropdown-click:hover > .w3-button:first-child{background-color:#ccc;color:#000}
        .w3-dropdown-content{cursor:auto;color:#000;background-color:#fff;display:none;position:absolute;min-width:160px;margin:0;padding:0;z-index:1}
        .w3-check,.w3-radio{width:24px;height:24px;position:relative;top:6px}
        .w3-sidebar{height:100%;width:200px;background-color:#fff;position:fixed!important;z-index:1;overflow:auto}
        .w3-bar-block .w3-dropdown-hover,.w3-bar-block .w3-dropdown-click{width:100%}
        .w3-bar-block .w3-dropdown-hover .w3-dropdown-content,.w3-bar-block .w3-dropdown-click .w3-dropdown-content{min-width:100%}
        .w3-bar-block .w3-dropdown-hover .w3-button,.w3-bar-block .w3-dropdown-click .w3-button{width:100%;text-align:left;padding:8px 16px}
        .w3-main,#main{transition:margin-left .4s}
        .w3-modal{z-index:3;display:none;padding-top:100px;position:fixed;left:0;top:0;width:100%;height:100%;overflow:auto;background-color:rgb(0,0,0);background-color:rgba(0,0,0,0.4)}
        .w3-modal-content{margin:auto;background-color:#fff;position:relative;padding:0;outline:0;width:600px}
        .w3-bar{width:100%;overflow:hidden}.w3-center .w3-bar{display:inline-block;width:auto}
        .w3-bar .w3-bar-item{padding:8px 16px;float:left;width:auto;border:none;display:block}
        .w3-bar .w3-dropdown-hover,.w3-bar .w3-dropdown-click{position:static;float:left}
        .w3-bar .w3-button{white-space:normal}
        .w3-bar-block .w3-bar-item{width:100%;display:block;padding:8px 16px;text-align:left;border:none;white-space:normal;float:none}
        .w3-bar-block.w3-center .w3-bar-item{text-align:center}.w3-block{display:block;width:100%}
        .w3-responsive{display:block;overflow-x:auto}
        .w3-container:after,.w3-container:before,.w3-panel:after,.w3-panel:before,.w3-row:after,.w3-row:before,.w3-row-padding:after,.w3-row-padding:before,
        .w3-cell-row:before,.w3-cell-row:after,.w3-clear:after,.w3-clear:before,.w3-bar:before,.w3-bar:after{content:'';display:table;clear:both}
        .w3-col,.w3-half,.w3-third,.w3-twothird,.w3-threequarter,.w3-quarter{float:left;width:100%}
        .w3-col.s1{width:8.33333%}.w3-col.s2{width:16.66666%}.w3-col.s3{width:24.99999%}.w3-col.s4{width:33.33333%}
        .w3-col.s5{width:41.66666%}.w3-col.s6{width:49.99999%}.w3-col.s7{width:58.33333%}.w3-col.s8{width:66.66666%}
        .w3-col.s9{width:74.99999%}.w3-col.s10{width:83.33333%}.w3-col.s11{width:91.66666%}.w3-col.s12{width:99.99999%}
        @media (min-width:601px){.w3-col.m1{width:8.33333%}.w3-col.m2{width:16.66666%}.w3-col.m3,.w3-quarter{width:24.99999%}.w3-col.m4,.w3-third{width:33.33333%}
        .w3-col.m5{width:41.66666%}.w3-col.m6,.w3-half{width:49.99999%}.w3-col.m7{width:58.33333%}.w3-col.m8,.w3-twothird{width:66.66666%}
        .w3-col.m9,.w3-threequarter{width:74.99999%}.w3-col.m10{width:83.33333%}.w3-col.m11{width:91.66666%}.w3-col.m12{width:99.99999%}}
        @media (min-width:993px){.w3-col.l1{width:8.33333%}.w3-col.l2{width:16.66666%}.w3-col.l3{width:24.99999%}.w3-col.l4{width:33.33333%}
        .w3-col.l5{width:41.66666%}.w3-col.l6{width:49.99999%}.w3-col.l7{width:58.33333%}.w3-col.l8{width:66.66666%}
        .w3-col.l9{width:74.99999%}.w3-col.l10{width:83.33333%}.w3-col.l11{width:91.66666%}.w3-col.l12{width:99.99999%}}
        .w3-content{max-width:980px;margin:auto}.w3-rest{overflow:hidden}
        .w3-cell-row{display:table;width:100%}.w3-cell{display:table-cell}
        .w3-cell-top{vertical-align:top}.w3-cell-middle{vertical-align:middle}.w3-cell-bottom{vertical-align:bottom}
        .w3-hide{display:none!important}.w3-show-block,.w3-show{display:block!important}.w3-show-inline-block{display:inline-block!important}
        @media (max-width:600px){.w3-modal-content{margin:0 10px;width:auto!important}.w3-modal{padding-top:30px}
        .w3-dropdown-hover.w3-mobile .w3-dropdown-content,.w3-dropdown-click.w3-mobile .w3-dropdown-content{position:relative}
        .w3-hide-small{display:none!important}.w3-mobile{display:block;width:100%!important}.w3-bar-item.w3-mobile,.w3-dropdown-hover.w3-mobile,.w3-dropdown-click.w3-mobile{text-align:center}
        .w3-dropdown-hover.w3-mobile,.w3-dropdown-hover.w3-mobile .w3-btn,.w3-dropdown-hover.w3-mobile .w3-button,.w3-dropdown-click.w3-mobile,.w3-dropdown-click.w3-mobile .w3-btn,.w3-dropdown-click.w3-mobile .w3-button{width:100%}}
        @media (max-width:768px){.w3-modal-content{width:500px}.w3-modal{padding-top:50px}}
        @media (min-width:993px){.w3-modal-content{width:900px}.w3-hide-large{display:none!important}.w3-sidebar.w3-collapse{display:block!important}}
        @media (max-width:992px) and (min-width:601px){.w3-hide-medium{display:none!important}}
        @media (max-width:992px){.w3-sidebar.w3-collapse{display:none}.w3-main{margin-left:0!important;margin-right:0!important}}
        .w3-top,.w3-bottom{position:fixed;width:100%;z-index:1}.w3-top{top:0}.w3-bottom{bottom:0}
        .w3-overlay{position:fixed;display:none;width:100%;height:100%;top:0;left:0;right:0;bottom:0;background-color:rgba(0,0,0,0.5);z-index:2}
        .w3-display-topleft{position:absolute;left:0;top:0}.w3-display-topright{position:absolute;right:0;top:0}
        .w3-display-bottomleft{position:absolute;left:0;bottom:0}.w3-display-bottomright{position:absolute;right:0;bottom:0}
        .w3-display-middle{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);-ms-transform:translate(-50%,-50%)}
        .w3-display-left{position:absolute;top:50%;left:0%;transform:translate(0%,-50%);-ms-transform:translate(-0%,-50%)}
        .w3-display-right{position:absolute;top:50%;right:0%;transform:translate(0%,-50%);-ms-transform:translate(0%,-50%)}
        .w3-display-topmiddle{position:absolute;left:50%;top:0;transform:translate(-50%,0%);-ms-transform:translate(-50%,0%)}
        .w3-display-bottommiddle{position:absolute;left:50%;bottom:0;transform:translate(-50%,0%);-ms-transform:translate(-50%,0%)}
        .w3-display-container:hover .w3-display-hover{display:block}.w3-display-container:hover span.w3-display-hover{display:inline-block}.w3-display-hover{display:none}
        .w3-display-position{position:absolute}
        .w3-circle{border-radius:50%}
        .w3-round-small{border-radius:2px}.w3-round,.w3-round-medium{border-radius:4px}.w3-round-large{border-radius:8px}.w3-round-xlarge{border-radius:16px}.w3-round-xxlarge{border-radius:32px}
        .w3-row-padding,.w3-row-padding>.w3-half,.w3-row-padding>.w3-third,.w3-row-padding>.w3-twothird,.w3-row-padding>.w3-threequarter,.w3-row-padding>.w3-quarter,.w3-row-padding>.w3-col{padding:0 8px}
        .w3-container,.w3-panel{padding:0.01em 16px}.w3-panel{margin-top:16px;margin-bottom:16px}
        .w3-code,.w3-codespan{font-family:Consolas,'courier new';font-size:16px}
        .w3-code{width:auto;background-color:#fff;padding:8px 12px;border-left:4px solid #4CAF50;word-wrap:break-word}
        .w3-codespan{color:crimson;background-color:#f1f1f1;padding-left:4px;padding-right:4px;font-size:110%}
        .w3-card,.w3-card-2{box-shadow:0 2px 5px 0 rgba(0,0,0,0.16),0 2px 10px 0 rgba(0,0,0,0.12)}
        .w3-card-4,.w3-hover-shadow:hover{box-shadow:0 4px 10px 0 rgba(0,0,0,0.2),0 4px 20px 0 rgba(0,0,0,0.19)}
        .w3-spin{animation:w3-spin 2s infinite linear}@keyframes w3-spin{0%{transform:rotate(0deg)}100%{transform:rotate(359deg)}}
        .w3-animate-fading{animation:fading 10s infinite}@keyframes fading{0%{opacity:0}50%{opacity:1}100%{opacity:0}}
        .w3-animate-opacity{animation:opac 0.8s}@keyframes opac{from{opacity:0} to{opacity:1}}
        .w3-animate-top{position:relative;animation:animatetop 0.4s}@keyframes animatetop{from{top:-300px;opacity:0} to{top:0;opacity:1}}
        .w3-animate-left{position:relative;animation:animateleft 0.4s}@keyframes animateleft{from{left:-300px;opacity:0} to{left:0;opacity:1}}
        .w3-animate-right{position:relative;animation:animateright 0.4s}@keyframes animateright{from{right:-300px;opacity:0} to{right:0;opacity:1}}
        .w3-animate-bottom{position:relative;animation:animatebottom 0.4s}@keyframes animatebottom{from{bottom:-300px;opacity:0} to{bottom:0;opacity:1}}
        .w3-animate-zoom {animation:animatezoom 0.6s}@keyframes animatezoom{from{transform:scale(0)} to{transform:scale(1)}}
        .w3-animate-input{transition:width 0.4s ease-in-out}.w3-animate-input:focus{width:100%!important}
        .w3-opacity,.w3-hover-opacity:hover{opacity:0.60}.w3-opacity-off,.w3-hover-opacity-off:hover{opacity:1}
        .w3-opacity-max{opacity:0.25}.w3-opacity-min{opacity:0.75}
        .w3-greyscale-max,.w3-grayscale-max,.w3-hover-greyscale:hover,.w3-hover-grayscale:hover{filter:grayscale(100%)}
        .w3-greyscale,.w3-grayscale{filter:grayscale(75%)}.w3-greyscale-min,.w3-grayscale-min{filter:grayscale(50%)}
        .w3-sepia{filter:sepia(75%)}.w3-sepia-max,.w3-hover-sepia:hover{filter:sepia(100%)}.w3-sepia-min{filter:sepia(50%)}
        .w3-tiny{font-size:10px!important}.w3-small{font-size:12px!important}.w3-medium{font-size:15px!important}.w3-large{font-size:18px!important}
        .w3-xlarge{font-size:24px!important}.w3-xxlarge{font-size:36px!important}.w3-xxxlarge{font-size:48px!important}.w3-jumbo{font-size:64px!important}
        .w3-left-align{text-align:left!important}.w3-right-align{text-align:right!important}.w3-justify{text-align:justify!important}.w3-center{text-align:center!important}
        .w3-border-0{border:0!important}.w3-border{border:1px solid #ccc!important}
        .w3-border-top{border-top:1px solid #ccc!important}.w3-border-bottom{border-bottom:1px solid #ccc!important}
        .w3-border-left{border-left:1px solid #ccc!important}.w3-border-right{border-right:1px solid #ccc!important}
        .w3-topbar{border-top:6px solid #ccc!important}.w3-bottombar{border-bottom:6px solid #ccc!important}
        .w3-leftbar{border-left:6px solid #ccc!important}.w3-rightbar{border-right:6px solid #ccc!important}
        .w3-section,.w3-code{margin-top:16px!important;margin-bottom:16px!important}
        .w3-margin{margin:16px!important}.w3-margin-top{margin-top:16px!important}.w3-margin-bottom{margin-bottom:16px!important}
        .w3-margin-left{margin-left:16px!important}.w3-margin-right{margin-right:16px!important}
        .w3-padding-small{padding:4px 8px!important}.w3-padding{padding:8px 16px!important}.w3-padding-large{padding:12px 24px!important}
        .w3-padding-16{padding-top:16px!important;padding-bottom:16px!important}.w3-padding-24{padding-top:24px!important;padding-bottom:24px!important}
        .w3-padding-32{padding-top:32px!important;padding-bottom:32px!important}.w3-padding-48{padding-top:48px!important;padding-bottom:48px!important}
        .w3-padding-64{padding-top:64px!important;padding-bottom:64px!important}
        .w3-left{float:left!important}.w3-right{float:right!important}
        .w3-button:hover{color:#000!important;background-color:#00337e}
        .w3-transparent,.w3-hover-none:hover{background-color:transparent!important}
        .w3-hover-none:hover{box-shadow:none!important}
        /* Colors */
        .w3-amber,.w3-hover-amber:hover{color:#000!important;background-color:#ffc107!important}
        .w3-aqua,.w3-hover-aqua:hover{color:#000!important;background-color:#00ffff!important}
        .w3-blue,.w3-hover-blue:hover{color:#fff!important;background-color:#2196F3!important}
        .w3-light-blue,.w3-hover-light-blue:hover{color:#000!important;background-color:#87CEEB!important}
        .w3-brown,.w3-hover-brown:hover{color:#fff!important;background-color:#795548!important}
        .w3-cyan,.w3-hover-cyan:hover{color:#000!important;background-color:#00bcd4!important}
        .w3-blue-grey,.w3-hover-blue-grey:hover,.w3-blue-gray,.w3-hover-blue-gray:hover{color:#fff!important;background-color:#607d8b!important}
        .w3-green,.w3-hover-green:hover{color:#fff!important;background-color:#4CAF50!important}
        .w3-light-green,.w3-hover-light-green:hover{color:#000!important;background-color:#8bc34a!important}
        .w3-indigo,.w3-hover-indigo:hover{color:#fff!important;background-color:#3f51b5!important}
        .w3-khaki,.w3-hover-khaki:hover{color:#000!important;background-color:#f0e68c!important}
        .w3-lime,.w3-hover-lime:hover{color:#000!important;background-color:#cddc39!important}
        .w3-orange,.w3-hover-orange:hover{color:#000!important;background-color:#ff9800!important}
        .w3-deep-orange,.w3-hover-deep-orange:hover{color:#fff!important;background-color:#ff5722!important}
        .w3-pink,.w3-hover-pink:hover{color:#fff!important;background-color:#e91e63!important}
        .w3-purple,.w3-hover-purple:hover{color:#fff!important;background-color:#9c27b0!important}
        .w3-deep-purple,.w3-hover-deep-purple:hover{color:#fff!important;background-color:#673ab7!important}
        .w3-red,.w3-hover-red:hover{color:#fff!important;background-color:#f44336!important}
        .w3-sand,.w3-hover-sand:hover{color:#000!important;background-color:#fdf5e6!important}
        .w3-teal,.w3-hover-teal:hover{color:#fff!important;background-color:#009688!important}
        .w3-yellow,.w3-hover-yellow:hover{color:#000!important;background-color:#ffeb3b!important}
        .w3-white,.w3-hover-white:hover{color:#000!important;background-color:#fff!important}
        .w3-black,.w3-hover-black:hover{color:#fff!important;background-color:#000!important}
        .w3-grey,.w3-hover-grey:hover,.w3-gray,.w3-hover-gray:hover{color:#000!important;background-color:#9e9e9e!important}
        .w3-light-grey,.w3-hover-light-grey:hover,.w3-light-gray,.w3-hover-light-gray:hover{color:#000!important;background-color:#f1f1f1!important}
        .w3-dark-grey,.w3-hover-dark-grey:hover,.w3-dark-gray,.w3-hover-dark-gray:hover{color:#fff!important;background-color:#616161!important}
        .w3-pale-red,.w3-hover-pale-red:hover{color:#000!important;background-color:#ffdddd!important}
        .w3-pale-green,.w3-hover-pale-green:hover{color:#000!important;background-color:#ddffdd!important}
        .w3-pale-yellow,.w3-hover-pale-yellow:hover{color:#000!important;background-color:#ffffcc!important}
        .w3-pale-blue,.w3-hover-pale-blue:hover{color:#000!important;background-color:#ddffff!important}
        .w3-text-amber,.w3-hover-text-amber:hover{color:#ffc107!important}
        .w3-text-aqua,.w3-hover-text-aqua:hover{color:#00ffff!important}
        .w3-text-blue,.w3-hover-text-blue:hover{color:#2196F3!important}
        .w3-text-light-blue,.w3-hover-text-light-blue:hover{color:#87CEEB!important}
        .w3-text-brown,.w3-hover-text-brown:hover{color:#795548!important}
        .w3-text-cyan,.w3-hover-text-cyan:hover{color:#00bcd4!important}
        .w3-text-blue-grey,.w3-hover-text-blue-grey:hover,.w3-text-blue-gray,.w3-hover-text-blue-gray:hover{color:#607d8b!important}
        .w3-text-green,.w3-hover-text-green:hover{color:#4CAF50!important}
        .w3-text-light-green,.w3-hover-text-light-green:hover{color:#8bc34a!important}
        .w3-text-indigo,.w3-hover-text-indigo:hover{color:#3f51b5!important}
        .w3-text-khaki,.w3-hover-text-khaki:hover{color:#b4aa50!important}
        .w3-text-lime,.w3-hover-text-lime:hover{color:#cddc39!important}
        .w3-text-orange,.w3-hover-text-orange:hover{color:#ff9800!important}
        .w3-text-deep-orange,.w3-hover-text-deep-orange:hover{color:#ff5722!important}
        .w3-text-pink,.w3-hover-text-pink:hover{color:#e91e63!important}
        .w3-text-purple,.w3-hover-text-purple:hover{color:#9c27b0!important}
        .w3-text-deep-purple,.w3-hover-text-deep-purple:hover{color:#673ab7!important}
        .w3-text-red,.w3-hover-text-red:hover{color:#f44336!important}
        .w3-text-sand,.w3-hover-text-sand:hover{color:#fdf5e6!important}
        .w3-text-teal,.w3-hover-text-teal:hover{color:#009688!important}
        .w3-text-yellow,.w3-hover-text-yellow:hover{color:#d2be0e!important}
        .w3-text-white,.w3-hover-text-white:hover{color:#fff!important}
        .w3-text-black,.w3-hover-text-black:hover{color:#000!important}
        .w3-text-grey,.w3-hover-text-grey:hover,.w3-text-gray,.w3-hover-text-gray:hover{color:#757575!important}
        .w3-text-light-grey,.w3-hover-text-light-grey:hover,.w3-text-light-gray,.w3-hover-text-light-gray:hover{color:#f1f1f1!important}
        .w3-text-dark-grey,.w3-hover-text-dark-grey:hover,.w3-text-dark-gray,.w3-hover-text-dark-gray:hover{color:#3a3a3a!important}
        .w3-border-amber,.w3-hover-border-amber:hover{border-color:#ffc107!important}
        .w3-border-aqua,.w3-hover-border-aqua:hover{border-color:#00ffff!important}
        .w3-border-blue,.w3-hover-border-blue:hover{border-color:#2196F3!important}
        .w3-border-light-blue,.w3-hover-border-light-blue:hover{border-color:#87CEEB!important}
        .w3-border-brown,.w3-hover-border-brown:hover{border-color:#795548!important}
        .w3-border-cyan,.w3-hover-border-cyan:hover{border-color:#00bcd4!important}
        .w3-border-blue-grey,.w3-hover-border-blue-grey:hover,.w3-border-blue-gray,.w3-hover-border-blue-gray:hover{border-color:#607d8b!important}
        .w3-border-green,.w3-hover-border-green:hover{border-color:#4CAF50!important}
        .w3-border-light-green,.w3-hover-border-light-green:hover{border-color:#8bc34a!important}
        .w3-border-indigo,.w3-hover-border-indigo:hover{border-color:#3f51b5!important}
        .w3-border-khaki,.w3-hover-border-khaki:hover{border-color:#f0e68c!important}
        .w3-border-lime,.w3-hover-border-lime:hover{border-color:#cddc39!important}
        .w3-border-orange,.w3-hover-border-orange:hover{border-color:#ff9800!important}
        .w3-border-deep-orange,.w3-hover-border-deep-orange:hover{border-color:#ff5722!important}
        .w3-border-pink,.w3-hover-border-pink:hover{border-color:#e91e63!important}
        .w3-border-purple,.w3-hover-border-purple:hover{border-color:#9c27b0!important}
        .w3-border-deep-purple,.w3-hover-border-deep-purple:hover{border-color:#673ab7!important}
        .w3-border-red,.w3-hover-border-red:hover{border-color:#f44336!important}
        .w3-border-sand,.w3-hover-border-sand:hover{border-color:#fdf5e6!important}
        .w3-border-teal,.w3-hover-border-teal:hover{border-color:#009688!important}
        .w3-border-yellow,.w3-hover-border-yellow:hover{border-color:#ffeb3b!important}
        .w3-border-white,.w3-hover-border-white:hover{border-color:#fff!important}
        .w3-border-black,.w3-hover-border-black:hover{border-color:#000!important}
        .w3-border-grey,.w3-hover-border-grey:hover,.w3-border-gray,.w3-hover-border-gray:hover{border-color:#9e9e9e!important}
        .w3-border-light-grey,.w3-hover-border-light-grey:hover,.w3-border-light-gray,.w3-hover-border-light-gray:hover{border-color:#f1f1f1!important}
        .w3-border-dark-grey,.w3-hover-border-dark-grey:hover,.w3-border-dark-gray,.w3-hover-border-dark-gray:hover{border-color:#616161!important}
        .w3-border-pale-red,.w3-hover-border-pale-red:hover{border-color:#ffe7e7!important}.w3-border-pale-green,.w3-hover-border-pale-green:hover{border-color:#e7ffe7!important}
        .w3-border-pale-yellow,.w3-hover-border-pale-yellow:hover{border-color:#ffffcc!important}.w3-border-pale-blue,.w3-hover-border-pale-blue:hover{border-color:#e7ffff!important}"""

style = """
/* OpenPLC Style */
        .top {
            position:absolute;
            left:0; right:0; top:0;
            height: 50px;
            background-color: #000000;
            position: fixed;
            overflow: hidden;
            z-index: 10
        }
        
        .main {
            position: absolute;
            left:0px; top:50px; right:0; bottom:0;
        }
        
        .user {
            position:absolute;
            left:75%; right:0; top:0;
            height: 50px;
            position: fixed;
            overflow: hidden;
            z-index: 11;
            text-align:right;
        }
        
        .button {
            background-color: #0066FC;
            border: 1px solid #1F1F1F;
            border-radius: 4px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-family: "Roboto", sans-serif;
        }
        
        .button:hover {
            background-color: #00337e;
        }
        
        table, h1, h2, h3, p {
            font-family: "Roboto", sans-serif;
            border-collapse: collapse;
            width: 100%;
        }
        
        td, th {
            border: 1px solid #cccccc;
            text-align: left;
            padding: 8px;
        }
        
        tr:nth-child(even) {
            background-color: #eeeeee;
        }
        
        tr:hover {
            cursor: hand;background-color: slategray;
        }
        
        label {
            font-family: arial, sans-serif;
        }
        
        input[type=text], input[type=password], select, textarea {
            width: 100%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
            font-family: "Roboto", sans-serif;
        }
        </style>
        <body>
        """
        
        
dashboard_head = """
        .top {
            position:absolute;
            left:0; right:0; top:0;
            height: 50px;
            background-color: #000000;
            position: fixed;
            overflow: hidden;
            z-index: 10
        }
        
       .main {
            position: absolute;
            left:0px; top:50px; right:0; bottom:0;
        }
        
       .user {
            position:absolute;
            left:75%; right:0; top:0;
            height: 50px;
            position: fixed;
            overflow: hidden;
            z-index: 11;
            text-align:right;
        }
        
       .button {
            background-color: #0066FC;
            border: 1px solid #1F1F1F;
            border-radius: 4px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-family: 'Roboto', sans-serif;
        }
        
       .button:hover {
            background-color: #00337e;
        }
        
       .copyButton {
            border: 1px solid #1F1F1F;
            border-radius: 4px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 20px;
            font-family: 'Roboto', sans-serif;
            height:50px;
            font-size:16px; 
            padding-top:11px; 
            background-color: #7D7D7D;
        }
        
       .copyButton:hover {
            background-color: #3D3D3D;
        }
        
       textarea {
            width: 96%;
            height: 350px;
            padding: 12px 20px;
            box-sizing: border-box;
            border: 2px solid #ccc;
            border-radius: 4px;
            background-color: #f8f8f8;
            font-size: 16px;
            font-family: courier;
            resize: none;
        }
        
       .tooltip {
            position: relative;
            display: inline-block;
        }

       .tooltip .tooltiptext {
            visibility: hidden;
            width: 140px;
            background-color: #555;
            color: #fff;
            text-align: center;
            border-radius: 6px;
            padding: 5px;
            position: absolute;
            z-index: 1;
            bottom: 150%;
            left: 50%;
            margin-left: -75px;
            opacity: 0;
            transition: opacity 0.3s;
        }

       .tooltip .tooltiptext::after {
            content: '';
            position: absolute;
            top: 100%;
            left: 50%;
            margin-left: -5px;
            border-width: 5px;
            border-style: solid;
            border-color: #555 transparent transparent transparent;
        }

       .tooltip:hover .tooltiptext {
            visibility: visible;
            opacity: 1;
        }
        
    </style>
    <body onload='loadData()'>"""

dashboard_tail = """
                    <br>
                    <br>
                    <h2>Runtime Logs</h2>
                    <textarea wrap='off' spellcheck='false' id='mytextarea'>
loading logs...
                    </textarea>
                    <div class='tooltip' style='margin: 5px 0px 0px 0px'>
                        <button class='button' style='height:45px; padding-top:10px' onclick='copyClipboard()' onmouseout='outFunc()'><span class='tooltiptext' id='myTooltip'>Copy to clipboard</span>Copy logs</button>
                    </div>
                        
                </div>
            </div>
        </div>
    </body>
    
    <script>
        var mytext = document.getElementById('mytextarea');
        mytext.scrollTop = document.getElementById('mytextarea').scrollHeight;
        var req;
        
        function copyClipboard() 
        {
            var copyText = document.getElementById('mytextarea');
            copyText.select();
            document.execCommand('Copy');
            
            var tooltip = document.getElementById('myTooltip');
            tooltip.innerHTML = 'Copied!';
        }
        
        function outFunc() 
        {
            var tooltip = document.getElementById('myTooltip');
            tooltip.innerHTML = 'Copy to clipboard';
        }
        
        function loadData()
        {
            url = 'runtime_logs'
            try
            {
                req = new XMLHttpRequest();
            } catch (e) 
            {
                try
                {
                    req = new ActiveXObject('Msxml2.XMLHTTP');
                } catch (e) 
                {
                    try 
                    {
                        req = new ActiveXObject('Microsoft.XMLHTTP');
                    } catch (oc) 
                    {
                        alert('No AJAX Support');
                        return;
                    }
                }
            }
            
            req.onreadystatechange = processReqChange;
            req.open('GET', url, true);
            req.send(null);
        }
        
        function processReqChange()
        {
            //If req shows 'complete'
            if (req.readyState == 4)
            {
                runtime_logs = document.getElementById('mytextarea');
                
                //If 'OK'
                if (req.status == 200)
                {
                    //Update textarea text
                    runtime_logs.value = req.responseText;
                    
                    //Start a new update timer
                    timeoutID = setTimeout('loadData()', 1000);
                }
                else
                {
                    runtime_logs.value = 'There was a problem retrieving the logs. Error: ' + req.statusText;
                }
            }
        }
        
    </script>
</html>"""

monitoring_head = """
/* OpenPLC Style */
        .top {
            position:absolute;
            left:0; right:0; top:0;
            height: 50px;
            background-color: #000000;
            position: fixed;
            overflow: hidden;
            z-index: 10
        }
        
        .main {
            position: absolute;
            left:0px; top:50px; right:0; bottom:0;
        }
        
        .user {
            position:absolute;
            left:75%; right:0; top:0;
            height: 50px;
            position: fixed;
            overflow: hidden;
            z-index: 11;
            text-align:right;
        }
        
        .button {
            background-color: #0066FC;
            border: 1px solid #1F1F1F;
            border-radius: 4px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-family: "Roboto", sans-serif;
        }
        
        .button:hover {
            background-color: #00337e;
        }
        
        table, h1, h2, h3, p {
            font-family: "Roboto", sans-serif;
            border-collapse: collapse;
            width: 100%;
        }
        
        td, th {
            border: 1px solid #cccccc;
            text-align: left;
            padding: 8px;
        }
        
        tr:nth-child(even) {
            background-color: #eeeeee;
        }
        
        tr:hover {
            cursor: hand;background-color: slategray;
        }
        
        label {
            font-family: arial, sans-serif;
        }
        
        .form-inline 
        {  
            display: flex;
            flex-flow: row wrap;
            align-items: center;
        }

        .form-inline label 
        {
            margin: 5px 10px 5px 0;
            width: 130px;
        }

        .form-inline input 
        {
            vertical-align: middle;
            margin: 5px 10px 5px 0;
            padding: 10px;
            width: calc(100% - 250px);
            background-color: #fff;
            border: 1px solid #ddd;
        }

        .form-inline button 
        {
            padding: 10px 20px;
            background-color: #0066FC;
            width: 100px;
            border: 1px solid #1F1F1F;
            color: white;
            cursor: pointer;
            font-size: 14px;
            font-family: "Roboto", sans-serif;
        }

        .form-inline button:hover 
        {
            background-color: #00337e;
        }

        @media (max-width: 800px) 
        {
            .form-inline input 
            {
                margin: 10px 0;
            }

            .form-inline 
            {
                flex-direction: column;
                align-items: stretch;
            }
        }
        </style>
        <body onload='loadData()'>"""

        
monitoring_tail = """
                </div>
            </div>
        </div>
    </body>
    
    <script>
        var req;
        var refresh_rate = 100;
        
        function loadData()
        {
            html_modbus_port = document.getElementById('modbus_port_cfg');
            url = 'monitor-update?mb_port=' + html_modbus_port.value;
            try
            {
                req = new XMLHttpRequest();
            } catch (e) 
            {
                try
                {
                    req = new ActiveXObject('Msxml2.XMLHTTP');
                } catch (e) 
                {
                    try 
                    {
                        req = new ActiveXObject('Microsoft.XMLHTTP');
                    } catch (oc) 
                    {
                        alert('No AJAX Support');
                        return;
                    }
                }
            }
            
            req.onreadystatechange = processReqChange;
            req.open('GET', url, true);
            req.send(null);
        }
        
        function updateRefreshRate()
        {
            html_refresh_text = document.getElementById('refresh_rate');
            refresh_rate = parseInt(html_refresh_text.value);
            if (refresh_rate < 100)
            {
                refresh_rate = 100
            }
            
            html_refresh_text.value = refresh_rate;
        }
        
        function processReqChange()
        {
            //If req shows 'complete'
            if (req.readyState == 4)
            {
                mon_table = document.getElementById('monitor_table');
                
                //If 'OK'
                if (req.status == 200)
                {
                    //Update table contents
                    mon_table.innerHTML = req.responseText;
                    
                    //Start a new update timer
                    timeoutID = setTimeout('loadData()', refresh_rate);
                }
            }
        }
    </script>
</html>"""

point_info_tail = """
                        <br>
                        <br>
                        <br>
                        <br>
                        <center><input type="submit" value="Save Changes" class="button" style="width: 310px; height: 53px; margin: 0px 20px 0px 20px;"></center>
                    </form>
                </div>
            </div>
        </div>
    </body>
    
    <script type="text/javascript">
        var req;
        
        window.onload = function()
        {
            setupSelector();
            loadData();
        }
        
        function setupSelector()
        {
            var checkbox_element = document.getElementById('force_checkbox');
            var selector_element = document.getElementById('forced_value');
            if (checkbox_element.checked == true)
            {
                selector_element.disabled = false;
            }
            else
            {
                selector_element.disabled = true;
            }
        }

        document.getElementById('force_checkbox').onchange = function()
        {
            setupSelector();
        }
        
        function loadData()
        {
            table_id = document.getElementById('point_id').value;
            url = 'point-update?table_id=' + table_id;
            try
            {
                req = new XMLHttpRequest();
            } catch (e) 
            {
                try
                {
                    req = new ActiveXObject('Msxml2.XMLHTTP');
                } catch (e) 
                {
                    try 
                    {
                        req = new ActiveXObject('Microsoft.XMLHTTP');
                    } catch (oc) 
                    {
                        alert('No AJAX Support');
                        return;
                    }
                }
            }
            
            req.onreadystatechange = processReqChange;
            req.open('GET', url, true);
            req.send(null);
        }
        
        function processReqChange()
        {
            //If req shows 'complete'
            if (req.readyState == 4)
            {
                mon_point = document.getElementById('monitor_point');
                
                //If 'OK'
                if (req.status == 200)
                {
                    //Update table contents
                    mon_point.innerHTML = req.responseText;
                    
                    //Start a new update timer
                    timeoutID = setTimeout('loadData()', 500);
                }
            }
        }
    </script>
</html>"""

add_user_tail = """
            </div>
            <div style="margin-left:320px; margin-right:70px">
                <div style="w3-container">
                    <br>
                    <h2>Add User</h2>
                    <br>
                    <form   id    = "uploadForm"
                        enctype   =  "multipart/form-data"
                        action    =  "add-user"
                        method    =  "post"
                        onsubmit  =  "return validateForm()">
                        <label for='full_name'><b>Name</b></label>
                        <input type='text' id='full_name' name='full_name' placeholder='John Appleseed'>
                        <label for='user_name'><b>Username</b></label>
                        <input type='text' id='user_name' name='user_name' placeholder='username'>
                        <label for='user_email'><b>Email</b></label>
                        <input type='text' id='user_email' name='user_email' placeholder='your@email.com'>
                        <label for='user_password'><b>Password</b></label>
                        <input type='password' id='user_password' name='user_password' placeholder='password'>
                        <label for='uploadForm'><b>Picture</b></label>
                        <br>
                        <br>
                        <input type="file" name="file" id="file" class="inputfile" accept="image/*">
                        <br>
                        <br>
                        <center><input type="submit" class="button" style="font-weight:bold; width: 310px; height: 53px; margin: 0px 20px 0px 20px;" value='Save user'></center>
                    </form>
                </div>
            </div>
        </div>
    </body>
    <script>
        function validateForm()
        {
            var username = document.forms["uploadForm"]["user_name"].value;
            var password = document.forms["uploadForm"]["user_password"].value;
            if (username == "" || password == "")
            {
                alert("Username and Password cannot be blank");
                return false;
            }
            return true;
        }
    </script>
</html>"""

settings_style = """
        /* OpenPLC Style */
        .top {
            position:absolute;
            left:0; right:0; top:0;
            height: 50px;
            background-color: #000000;
            position: fixed;
            overflow: hidden;
            z-index: 10
        }
        
        .main {
            position: absolute;
            left:0px; top:50px; right:0; bottom:0;
        }
        
        .user {
            position:absolute;
            left:75%; right:0; top:0;
            height: 50px;
            position: fixed;
            overflow: hidden;
            z-index: 11;
            text-align:right;
        }
        
        .button {
            background-color: #0066FC;
            border: 1px solid #1F1F1F;
            border-radius: 4px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-family: "Roboto", sans-serif;
            font-weight: bold;
        }
        
        .button:hover {
            background-color: #00337e;
        }
        
        /*Custom Checkbox */
        
        /* The container */
        .container {
            display: block;
            position: relative;
            padding-left: 35px;
            margin-bottom: 12px;
            cursor: pointer;
            font-size: 16px;
            font-family: "Roboto", sans-serif;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            width: 300px;
        }

        /* Hide the browser's default checkbox */
        .container input {
            position: absolute;
            opacity: 0;
            cursor: pointer;
        }

        /* Create a custom checkbox */
        .checkmark {
            position: absolute;
            top: 0;
            left: 0;
            height: 25px;
            width: 25px;
            background-color: #eee;
        }

        /* On mouse-over, add a grey background color */
        .container:hover input ~ .checkmark {
            background-color: #ccc;
        }

        /* When the checkbox is checked, add a blue background */
        .container input:checked ~ .checkmark {
            background-color: #0066FC;
        }

        /* Create the checkmark/indicator (hidden when not checked) */
        .checkmark:after {
            content: "";
            position: absolute;
            display: none;
        }

        /* Show the checkmark when checked */
        .container input:checked ~ .checkmark:after {
            display: block;
        }

        /* Style the checkmark/indicator */
        .container .checkmark:after {
            left: 9px;
            top: 5px;
            width: 5px;
            height: 10px;
            border: solid white;
            border-width: 0 3px 3px 0;
            -webkit-transform: rotate(45deg);
            -ms-transform: rotate(45deg);
            transform: rotate(45deg);
        }
    </style>
    
    <body>"""

settings_head = """
        <div class='main'>
            <div class="w3-sidebar w3-bar-block" style="width:250px; background-color:#1F1F1F">
                <br>
                <br>
                <a href="dashboard" class="w3-bar-item w3-button"><img src="/static/home-icon-64x64.png" alt="Dashboard" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Dashboard</p></a>
                <a href='programs' class='w3-bar-item w3-button'><img src='/static/programs-icon-64x64.png' alt='Programs' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Programs</p></a>
                <a href='modbus' class='w3-bar-item w3-button'><img src='/static/modbus-icon-512x512.png' alt='Modbus' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Slave Devices</p></a>
                <a href='monitoring' class='w3-bar-item w3-button'><img src='/static/monitoring-icon-64x64.png' alt='Monitoring' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Monitoring</p></a>
                <a href='hardware' class='w3-bar-item w3-button'><img src='/static/hardware-icon-980x974.png' alt='Hardware' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Hardware</p></a>
                <a href="users" class="w3-bar-item w3-button"><img src="/static/users-icon-64x64.png" alt="Users" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Users</p></a>
                <a href="settings" class="w3-bar-item w3-button" style="background-color:#0066FC; padding-right:0px;padding-top:0px;padding-bottom:0px"><img src="/static/settings-icon-64x64.png" alt="Settings" style="width:47px;height:39px;padding:7px 15px 0px 0px;float:left"><img src="/static/arrow.png" style="width:17px;height:49px;padding:0px 0px 0px 0px;margin: 0px 0px 0px 0px; float:right"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 10px 0px 0px 0px'>Settings</p></a>
                <a href='logout' class='w3-bar-item w3-button'><img src='/static/logout-icon-64x64.png' alt='Logout' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Logout</p></a>
                <br>
                <br>"""
                
settings_tail = """
                        <br>
                        <br>
                        <br>
                        <center><input type="submit" value="Save Changes" class="button" style="width: 310px; height: 53px; margin: 0px 20px 0px 20px;"></center>
                    </form>
                </div>
            </div>
        </div>
    </body>
    
    <script>
        window.onload = function()
        {
            setupCheckboxes();
        }
        
        function setupCheckboxes()
        {
            var modbus_checkbox = document.getElementById('modbus_server');
            var modbus_text = document.getElementById('modbus_server_port');
            var dnp3_checkbox = document.getElementById('dnp3_server');
            var dnp3_text = document.getElementById('dnp3_server_port');
            var enip_checkbox = document.getElementById('enip_server');
            var enip_text = document.getElementById('enip_server_port');
            var pstorage_checkbox = document.getElementById('pstorage_thread');
            var pstorage_text = document.getElementById('pstorage_thread_poll');
            var auto_run_checkbox = document.getElementById('auto_run');
            var auto_run_text = document.getElementById('auto_run_text');
            
            if (modbus_checkbox.checked == true)
            {
                modbus_text.disabled = false;
            }
            else
            {
                modbus_text.disabled = true;
            }
            
            if (dnp3_checkbox.checked == true)
            {
                dnp3_text.disabled = false;
            }
            else
            {
                dnp3_text.disabled = true;
            }
            
            if (enip_checkbox.checked == true)
            {
                enip_text.disabled = false;
            }
            else
            {
                enip_text.disabled = true;
            }
            
            if (pstorage_checkbox.checked == true)
            {
                pstorage_text.disabled = false;
            }
            else
            {
                pstorage_text.disabled = true;
            }
            
            if (auto_run_checkbox.checked == true)
            {
                auto_run_text.value = 'true';
            }
            else
            {
                auto_run_text.value = 'false';
            }
        }

        document.getElementById('modbus_server').onchange = function()
        {
            setupCheckboxes();
        }
        
        document.getElementById('dnp3_server').onchange = function()
        {
            setupCheckboxes();
        }
        
        document.getElementById('enip_server').onchange = function()
        {
            setupCheckboxes();
        }
        
        document.getElementById('pstorage_thread').onchange = function()
        {
            setupCheckboxes();
        }
        
        document.getElementById('auto_run').onchange = function()
        {
            setupCheckboxes();
        }
        
        function validateForm()
        {
            var modbus_checkbox = document.forms["uploadForm"]["modbus_server"].checked;
            var modbus_port = document.forms["uploadForm"]["modbus_server_port"].value;
            var dnp3_checkbox = document.forms["uploadForm"]["dnp3_server"].checked;
            var dnp3_port = document.forms["uploadForm"]["dnp3_server_port"].value;
            var enip_checkbox = document.forms["uploadForm"]["enip_server"].checked;
            var enip_port = document.forms["uploadForm"]["enip_server_port"].value;
            var pstorage_checkbox = document.forms["uploadForm"]["pstorage_thread"].checked;
            var pstorage_poll = document.forms["uploadForm"]["pstorage_thread_poll"].value;
            
            if (modbus_checkbox && (Number(modbus_port) < 0 || Number(modbus_port) > 65535))
            {
                alert("Please select a port number between 0 and 65535");
                return false;
            }
            if (dnp3_checkbox && (Number(dnp3_port) < 0 || Number(dnp3_port) > 65535))
            {
                alert("Please select a port number between 0 and 65535");
                return false;
            }
            if (enip_checkbox && (Number(enip_port) < 0 || Number(enip_port) > 65535))
            {
                alert("Please select a port number between 0 and 65535");
                return false;
            }
            if (pstorage_checkbox && Number(pstorage_poll) < 0)
            {
                alert("Persistent Storage polling rate must be bigger than zero");
                return false;
            }
            return true;
        }
    </script>
</html>"""

hardware_style = """
        /* OpenPLC Style */
        .top {
            position:absolute;
            left:0; right:0; top:0;
            height: 50px;
            background-color: #000000;
            position: fixed;
            overflow: hidden;
            z-index: 10
        }
        
        .main {
            position: absolute;
            left:0px; top:50px; right:0; bottom:0;
        }
        
        .user {
            position:absolute;
            left:75%; right:0; top:0;
            height: 50px;
            position: fixed;
            overflow: hidden;
            z-index: 11;
            text-align:right;
        }
        
        .button {
            background-color: #0066FC;
            border: 1px solid #1F1F1F;
            border-radius: 4px;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-family: "Roboto", sans-serif;
        }
        
        .button:hover {
            background-color: #00337e;
        }
        
        textarea {
            width: 100%;
            height: 450px;
            padding: 12px 20px;
            box-sizing: border-box;
            border: 2px solid #ccc;
            border-radius: 4px;
            background-color: #262626;
            font-size: 12px;
            font-family: courier;
            color: #A9875F;
            resize:vertical;
        }
        
        .CodeMirror {
            border: 1px;
            resize: vertical;
            overflow: auto !important;
        }
        
    </style>
    
    <script src="/static/codemirror/codemirror.js"></script>
    <link rel="stylesheet" href="/static/codemirror/codemirror.css">
    <link rel="stylesheet" href="/static/codemirror/liquibyte.css">
    <script src="/static/codemirror/javascript.js"></script>
    <script src="/static/codemirror/active-line.js"></script>
    <script src="/static/codemirror/matchbrackets.js"></script>
    
    <body onload='loadData()'>"""

hardware_head = """
        <div class='main'>
            <div class="w3-sidebar w3-bar-block" style="width:250px; background-color:#1F1F1F">
                <br>
                <br>
                <a href="dashboard" class="w3-bar-item w3-button"><img src="/static/home-icon-64x64.png" alt="Dashboard" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Dashboard</p></a>
                <a href='programs' class='w3-bar-item w3-button'><img src='/static/programs-icon-64x64.png' alt='Programs' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Programs</p></a>
                <a href='modbus' class='w3-bar-item w3-button'><img src='/static/modbus-icon-512x512.png' alt='Modbus' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Slave Devices</p></a>
                <a href='monitoring' class='w3-bar-item w3-button'><img src='/static/monitoring-icon-64x64.png' alt='Monitoring' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Monitoring</p></a>
                <a href="hardware" class="w3-bar-item w3-button" style="background-color:#0066FC; padding-right:0px;padding-top:0px;padding-bottom:0px"><img src="/static/hardware-icon-980x974.png" alt="Hardware" style="width:47px;height:39px;padding:7px 15px 0px 0px;float:left"><img src="/static/arrow.png" style="width:17px;height:49px;padding:0px 0px 0px 0px;margin: 0px 0px 0px 0px; float:right"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 10px 0px 0px 0px'>Hardware</p></a>
                <a href="users" class="w3-bar-item w3-button"><img src="/static/users-icon-64x64.png" alt="Users" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Users</p></a>
                <a href='settings' class='w3-bar-item w3-button'><img src='/static/settings-icon-64x64.png' alt='Settings' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Settings</p></a>
                <a href='logout' class='w3-bar-item w3-button'><img src='/static/logout-icon-64x64.png' alt='Logout' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Logout</p></a>
                <br>
                <br>"""

hardware_tail = """</textarea>
                        </div>
                        <br>
                        <br>
                        <center><input type="submit" class="button" style="font-weight:bold; width: 310px; height: 53px; margin: 0px 20px 0px 20px;" value="Save changes"><a href="restore_custom_hardware" id="code_restore" class="button" style="display:none; width: 310px; height: 53px; margin: 0px 20px 0px 20px;"><b>Restore Original Code</b></a></center>
                        <br>
                        <br>
                    </form>
                </div>
            </div>
        </div>
    </body>
    
    <script type="text/javascript">
    var myCodeMirror = CodeMirror.fromTextArea(custom_layer_code, 
    {
        lineNumbers: true,
        styleActiveLine: true,
        matchBrackets: true,
        theme: "liquibyte",
        indentUnit: 4,
        indentWithTabs: false,
        tabSize: 4,
        extraKeys: 
        {
            "Tab": function(cm)
            {
                cm.replaceSelection("    " , "end");
            }
        }
    });
    
    myCodeMirror.setSize(null, 450);
    
    //Insert a dummy script to load something from the server periodically so that the user cookie won't expire
    function loadData()
    {
        refreshSelector();
        url = 'runtime_logs'
        try
        {
            req = new XMLHttpRequest();
        } catch (e) 
        {
            try
            {
                req = new ActiveXObject('Msxml2.XMLHTTP');
            } catch (e) 
            {
                try 
                {
                    req = new ActiveXObject('Microsoft.XMLHTTP');
                } catch (oc) 
                {
                    alert('No AJAX Support');
                    return;
                }
            }
        }
        
        req.onreadystatechange = processReqChange;
        req.open('GET', url, true);
        req.send(null);
    }
    
    function processReqChange()
    {
        //If req shows 'complete'
        if (req.readyState == 4)
        {
            timeoutID = setTimeout('loadData()', 10000);
        }
    }
    
    function refreshSelector()
    {
        var drop_down = document.getElementById('hardware_layer');
        var psm_div = document.getElementById('psm_code');
        var restore_button = document.getElementById('code_restore');
        if (drop_down.value == "psm_linux" || drop_down.value == "psm_win")
        {
            restore_button.style.display = "inline";
            psm_div.style.visibility = "visible";
        }
        else
        {
            restore_button.style.display = "none";
            psm_div.style.visibility = "hidden";
        }
        
    }
    
    </script>
</html>"""

add_slave_devices_tail = """
                        </select>
                        <label for='dev_baud'><b>Baud Rate</b></label>
                        <input type='text' id='dev_baud' name='device_baud' placeholder='19200'>
                        <label for='dev_parity'><b>Parity</b></label>
                        <select id='dev_parity' name='device_parity'>
                            <option value='None'>None</option>
                            <option value='Even'>Even</option>
                            <option value='Odd'>Odd</option>
                        </select>
                        <label for='dev_data'><b>Data Bits</b></label>
                        <input type='text' id='dev_data' name='device_data' placeholder='8'>
                        <label for='dev_stop'><b>Stop Bits</b></label>
                        <input type='text' id='dev_stop' name='device_stop' placeholder='1'>
                        <label for='dev_pause'><b>Transmission Pause</b></label>
                        <input type='text' id='dev_pause' name='device_pause' placeholder='0'>
                        </div>
                    </div>
                    <div style="float:right; width:45%; height:780px">
                        <p style='font-size:20px; margin-top:0px'><b>Discrete Inputs (%IX100.0)</b></p>
                        <label for='di_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='di_start' name='di_start' placeholder='0'>
                        <label for='di_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='di_size' name='di_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Coils (%QX100.0)</b></p>
                        <label for='do_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='do_start' name='do_start' placeholder='0'>
                        <label for='do_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='do_size' name='do_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Input Registers (%IW100)</b></p>
                        <label for='ai_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='ai_start' name='ai_start' placeholder='0'>
                        <label for='ai_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='ai_size' name='ai_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Holding Registers - Read (%IW100)</b></p>
                        <label for='aor_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='aor_start' name='aor_start' placeholder='0'>
                        <label for='aor_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='aor_size' name='aor_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Holding Registers - Write (%QW100)</b></p>
                        <label for='aow_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='aow_start' name='aow_start' placeholder='0'>
                        <label for='aow_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='aow_size' name='aow_size' placeholder='8'>
                    </div>
                    <br>
                    <center><input type="submit" class="button" style="font-weight:bold; width: 310px; height: 53px; margin: 0px 20px 0px 20px;" value='Save device'></center>
                    </form>
                </div>
            </div>
        </div>
    </body>"""
    
add_devices_script = """
    <script type="text/javascript">
        window.onload = function()
        {
            setupPageContent()
            LoadValuesFromDB()
        }
        
        function turnElementOn(element)
        {
            element.readOnly = false
            element.value = ""
            element.style.backgroundColor = "white"
            element.style.color = "black"
        }
        
        function turnElementOff(element)
        {
            element.readOnly = true
            element.style.backgroundColor = "#F8F8F8"
            element.style.color = "#9C9C9C"
        }
        
        function setupPageContent()
        {
            var dropmenu = document.getElementById('dev_protocol');
            var tcpdiv = document.getElementById("tcp-stuff");
            var rtudiv = document.getElementById("rtu-stuff");
            
            var devport = document.getElementById("dev_port");
            var devid = document.getElementById("dev_id");
            
            var devbaud = document.getElementById("dev_baud");
            var devparity = document.getElementById("dev_parity");
            var devdata = document.getElementById("dev_data");
            var devstop = document.getElementById("dev_stop");
            var devpause = document.getElementById("dev_pause");
            
            var distart = document.getElementById("di_start");
            var disize = document.getElementById("di_size");
            var dostart = document.getElementById("do_start");
            var dosize = document.getElementById("do_size");
            var aistart = document.getElementById("ai_start");
            var aisize = document.getElementById("ai_size");
            var aorstart = document.getElementById("aor_start");
            var aorsize = document.getElementById("aor_size");
            var aowstart = document.getElementById("aow_start");
            var aowsize = document.getElementById("aow_size");
            
            if (dropmenu.options[dropmenu.selectedIndex].value=="TCP")
            {
                tcpdiv.style.display = "block"
                rtudiv.style.display = "none"
                
                turnElementOn(devport)
                turnElementOn(devid)
                turnElementOn(distart)
                turnElementOn(disize)
                turnElementOn(dostart)
                turnElementOn(dosize)
                turnElementOn(aistart)
                turnElementOn(aisize)
                turnElementOn(aorstart)
                turnElementOn(aorsize)
                turnElementOn(aowstart)
                turnElementOn(aowsize)
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="ESP32")
            {
                tcpdiv.style.display = "block"
                rtudiv.style.display = "none"
                
                turnElementOff(devport)
                devport.value = "502"
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "8"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "8"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "1"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "1"
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="ESP8266")
            {
                tcpdiv.style.display = "block"
                rtudiv.style.display = "none"
                
                turnElementOff(devport)
                devport.value = "502"
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "8"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "8"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "1"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "1"
            }
            
            else if (dropmenu.options[dropmenu.selectedIndex].value=="RTU") 
            {
                tcpdiv.style.display = "none"
                rtudiv.style.display = "block"
                
                turnElementOn(devid)
                turnElementOn(devbaud)
                turnElementOn(devparity)
                devparity.value = "None"
                turnElementOn(devdata)
                turnElementOn(devstop)
                turnElementOn(devpause)
                devpause.value = "0"
                turnElementOn(distart)
                turnElementOn(disize)
                turnElementOn(dostart)
                turnElementOn(dosize)
                turnElementOn(aistart)
                turnElementOn(aisize)
                turnElementOn(aorstart)
                turnElementOn(aorsize)
                turnElementOn(aowstart)
                turnElementOn(aowsize)
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="Uno")
            {
                tcpdiv.style.display = "none"
                rtudiv.style.display = "block"
                
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(devbaud)
                devbaud.value = "115200"
                turnElementOff(devparity)
                devparity.value = "None"
                turnElementOff(devdata)
                devdata.value = "8"
                turnElementOff(devstop)
                devstop.value = "1"
                turnElementOff(devpause)
                devpause.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "5"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "4"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "6"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "3"
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="Mega")
            {
                tcpdiv.style.display = "none"
                rtudiv.style.display = "block"
                
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(devbaud)
                devbaud.value = "115200"
                turnElementOff(devparity)
                devparity.value = "None"
                turnElementOff(devdata)
                devdata.value = "8"
                turnElementOff(devstop)
                devstop.value = "1"
                turnElementOff(devpause)
                devpause.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "24"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "16"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "16"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "12"
            }
        }
        
        document.getElementById('dev_protocol').onchange = function()
        {
            first_time_edit = false;
            setupPageContent()
        }
        
        function validateForm()
        {
            var devname = document.forms["uploadForm"]["dev_name"].value;
            var devid = document.forms["uploadForm"]["dev_id"].value;
            
            var devip = document.forms["uploadForm"]["dev_ip"].value;
            var devport = document.forms["uploadForm"]["dev_port"].value;
            
            var devbaud = document.forms["uploadForm"]["dev_baud"].value;
            var devdata = document.forms["uploadForm"]["dev_data"].value;
            var devstop = document.forms["uploadForm"]["dev_stop"].value;
            var devpause = document.forms["uploadForm"]["dev_pause"].value;
            
            var distart = document.forms["uploadForm"]["di_start"].value;
            var disize = document.forms["uploadForm"]["di_size"].value;
            var dostart = document.forms["uploadForm"]["do_start"].value;
            var dosize = document.forms["uploadForm"]["do_size"].value;
            var aistart = document.forms["uploadForm"]["ai_start"].value;
            var aisize = document.forms["uploadForm"]["ai_size"].value;
            var aorstart = document.forms["uploadForm"]["aor_start"].value;
            var aorsize = document.forms["uploadForm"]["aor_size"].value;
            var aowstart = document.forms["uploadForm"]["aow_start"].value;
            var aowsize = document.forms["uploadForm"]["aow_size"].value;
            
            
            if (devname == "" || devid == "" || distart == "" || disize == "" || dostart == "" || dosize == "" || aistart == "" || aisize == "" || aorstart == "" || aorsize == "" || aowstart == "" || aowsize == "")
            {
                alert("Please fill out all the fields before saving!");
                return false;
            }
            
            var dropmenu = document.getElementById('dev_protocol');
            var device_type = dropmenu.options[dropmenu.selectedIndex].value
            if (device_type=="TCP" || device_type=="ESP32" || device_type=="ESP8266")
            {
                if (devip == "" || devport == "")
                {
                    alert("Please fill out all the fields before saving!");
                    return false;
                }
            }
            else
            {
                if (devbaud == "" || devdata == "" || devstop == "")
                {
                    alert("Please fill out all the fields before saving!");
                    return false;
                }
            }
            
            return true;
        }
                
    </script>
</html>"""

edit_slave_devices_tail = """
                        </select>
                        <label for='dev_baud'><b>Baud Rate</b></label>
                        <input type='text' id='dev_baud' name='device_baud' placeholder='19200'>
                        <label for='dev_parity'><b>Parity</b></label>
                        <select id='dev_parity' name='device_parity'>
                            <option value='None'>None</option>
                            <option value='Even'>Even</option>
                            <option value='Odd'>Odd</option>
                        </select>
                        <label for='dev_data'><b>Data Bits</b></label>
                        <input type='text' id='dev_data' name='device_data' placeholder='8'>
                        <label for='dev_stop'><b>Stop Bits</b></label>
                        <input type='text' id='dev_stop' name='device_stop' placeholder='1'>
                        <label for='dev_pause'><b>Transmission Pause</b></label>
                        <input type='text' id='dev_pause' name='device_pause' placeholder='0'>
                        </div>
                    </div>
                    <div style="float:right; width:45%; height:780px">
                        <p style='font-size:20px; margin-top:0px'><b>Discrete Inputs (%IX100.0)</b></p>
                        <label for='di_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='di_start' name='di_start' placeholder='0'>
                        <label for='di_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='di_size' name='di_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Coils (%QX100.0)</b></p>
                        <label for='do_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='do_start' name='do_start' placeholder='0'>
                        <label for='do_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='do_size' name='do_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Input Registers (%IW100)</b></p>
                        <label for='ai_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='ai_start' name='ai_start' placeholder='0'>
                        <label for='ai_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='ai_size' name='ai_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Holding Registers - Read (%IW100)</b></p>
                        <label for='aor_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='aor_start' name='aor_start' placeholder='0'>
                        <label for='aor_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='aor_size' name='aor_size' placeholder='8'>
                        
                        <p style='font-size:20px'><b>Holding Registers - Write (%QW100)</b></p>
                        <label for='aow_start'><b>Start Address:</b></label>
                        <input type='text' style='width: 20%' id='aow_start' name='aow_start' placeholder='0'>
                        <label for='aow_size' style='padding-left:20px'><b>Size:</b></label>
                        <input type='text' style='width: 20%' id='aow_size' name='aow_size' placeholder='8'>
                    </div>
                    <br>
                    <center><input type="submit" class="button" style="font-weight:bold; width: 310px; height: 53px; margin: 0px 20px 0px 20px;" value='Save device'><a href='delete-device?dev_id="""

edit_devices_script = """
    <script type="text/javascript">
        window.onload = function()
        {
            setupPageContent()
            LoadValuesFromDB()
        }
        
        function turnElementOn(element)
        {
            element.readOnly = false
            element.value = ""
            element.style.backgroundColor = "white"
            element.style.color = "black"
        }
        
        function turnElementOff(element)
        {
            element.readOnly = true
            element.style.backgroundColor = "#F8F8F8"
            element.style.color = "#9C9C9C"
        }
        
        function setupPageContent()
        {
            var dropmenu = document.getElementById('dev_protocol');
            var tcpdiv = document.getElementById("tcp-stuff");
            var rtudiv = document.getElementById("rtu-stuff");
            
            var devport = document.getElementById("dev_port");
            var devid = document.getElementById("dev_id");
            
            var devbaud = document.getElementById("dev_baud");
            var devparity = document.getElementById("dev_parity");
            var devdata = document.getElementById("dev_data");
            var devstop = document.getElementById("dev_stop");
            var devpause = document.getElementById("dev_pause");
            
            var distart = document.getElementById("di_start");
            var disize = document.getElementById("di_size");
            var dostart = document.getElementById("do_start");
            var dosize = document.getElementById("do_size");
            var aistart = document.getElementById("ai_start");
            var aisize = document.getElementById("ai_size");
            var aorstart = document.getElementById("aor_start");
            var aorsize = document.getElementById("aor_size");
            var aowstart = document.getElementById("aow_start");
            var aowsize = document.getElementById("aow_size");
            
            if (dropmenu.options[dropmenu.selectedIndex].value=="TCP")
            {
                tcpdiv.style.display = "block"
                rtudiv.style.display = "none"
                
                turnElementOn(devport)
                turnElementOn(devid)
                turnElementOn(distart)
                turnElementOn(disize)
                turnElementOn(dostart)
                turnElementOn(dosize)
                turnElementOn(aistart)
                turnElementOn(aisize)
                turnElementOn(aorstart)
                turnElementOn(aorsize)
                turnElementOn(aowstart)
                turnElementOn(aowsize)
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="ESP32")
            {
                tcpdiv.style.display = "block"
                rtudiv.style.display = "none"
                
                turnElementOff(devport)
                devport.value = "502"
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "8"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "8"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "1"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "1"
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="ESP8266")
            {
                tcpdiv.style.display = "block"
                rtudiv.style.display = "none"
                
                turnElementOff(devport)
                devport.value = "502"
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "8"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "8"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "1"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "1"
            }
            
            else if (dropmenu.options[dropmenu.selectedIndex].value=="RTU") 
            {
                tcpdiv.style.display = "none"
                rtudiv.style.display = "block"
                
                turnElementOn(devid)
                turnElementOn(devbaud)
                turnElementOn(devparity)
                devparity.value = "None"
                turnElementOn(devdata)
                turnElementOn(devstop)
                turnElementOn(devpause)
                devpause.value = "0"
                turnElementOn(distart)
                turnElementOn(disize)
                turnElementOn(dostart)
                turnElementOn(dosize)
                turnElementOn(aistart)
                turnElementOn(aisize)
                turnElementOn(aorstart)
                turnElementOn(aorsize)
                turnElementOn(aowstart)
                turnElementOn(aowsize)
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="Uno")
            {
                tcpdiv.style.display = "none"
                rtudiv.style.display = "block"
                
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(devbaud)
                devbaud.value = "115200"
                turnElementOff(devparity)
                devparity.value = "None"
                turnElementOff(devdata)
                devdata.value = "8"
                turnElementOff(devstop)
                devstop.value = "1"
                turnElementOff(devpause)
                devpause.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "5"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "4"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "6"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "3"
            }
            else if (dropmenu.options[dropmenu.selectedIndex].value=="Mega")
            {
                tcpdiv.style.display = "none"
                rtudiv.style.display = "block"
                
                turnElementOff(devid)
                devid.value = "0"
                turnElementOff(devbaud)
                devbaud.value = "115200"
                turnElementOff(devparity)
                devparity.value = "None"
                turnElementOff(devdata)
                devdata.value = "8"
                turnElementOff(devstop)
                devstop.value = "1"
                turnElementOff(devpause)
                devpause.value = "0"
                turnElementOff(distart)
                distart.value = "0"
                turnElementOff(disize)
                disize.value = "24"
                turnElementOff(dostart)
                dostart.value = "0"
                turnElementOff(dosize)
                dosize.value = "16"
                turnElementOff(aistart)
                aistart.value = "0"
                turnElementOff(aisize)
                aisize.value = "16"
                turnElementOff(aorstart)
                aorstart.value = "0"
                turnElementOff(aorsize)
                aorsize.value = "0"
                turnElementOff(aowstart)
                aowstart.value = "0"
                turnElementOff(aowsize)
                aowsize.value = "12"
            }
        }
        
        document.getElementById('dev_protocol').onchange = function()
        {
            setupPageContent()
        }
        
        function validateForm()
        {
            var devname = document.forms["uploadForm"]["dev_name"].value;
            var devid = document.forms["uploadForm"]["dev_id"].value;
            
            var devip = document.forms["uploadForm"]["dev_ip"].value;
            var devport = document.forms["uploadForm"]["dev_port"].value;
            
            var devcport = document.forms["uploadForm"]["dev_cport"].value;
            var devbaud = document.forms["uploadForm"]["dev_baud"].value;
            var devdata = document.forms["uploadForm"]["dev_data"].value;
            var devstop = document.forms["uploadForm"]["dev_stop"].value;
            var devpause = document.forms["uploadForm"]["dev_pause"].value;
            
            var distart = document.forms["uploadForm"]["di_start"].value;
            var disize = document.forms["uploadForm"]["di_size"].value;
            var dostart = document.forms["uploadForm"]["do_start"].value;
            var dosize = document.forms["uploadForm"]["do_size"].value;
            var aistart = document.forms["uploadForm"]["ai_start"].value;
            var aisize = document.forms["uploadForm"]["ai_size"].value;
            var aorstart = document.forms["uploadForm"]["aor_start"].value;
            var aorsize = document.forms["uploadForm"]["aor_size"].value;
            var aowstart = document.forms["uploadForm"]["aow_start"].value;
            var aowsize = document.forms["uploadForm"]["aow_size"].value;
            
            
            if (devname == "" || devid == "" || distart == "" || disize == "" || dostart == "" || dosize == "" || aistart == "" || aisize == "" || aorstart == "" || aorsize == "" || aowstart == "" || aowsize == "")
            {
                alert("Please fill out all the fields before saving!");
                return false;
            }
            
            var dropmenu = document.getElementById('dev_protocol');
            var device_type = dropmenu.options[dropmenu.selectedIndex].value
            if (device_type=="TCP" || device_type=="ESP32" || device_type=="ESP8266")
            {
                if (devip == "" || devport == "")
                {
                    alert("Please fill out all the fields before saving!");
                    return false;
                }
            }
            else
            {
                if (devcport == "" || devbaud == "" || devdata == "" || devstop == "")
                {
                    alert("Please fill out all the fields before saving!");
                    return false;
                }
            }
            
            return true;
        }
        
        function LoadValuesFromDB()
        {
            var devid = document.getElementById("dev_id");
            var devip = document.getElementById("dev_ip");
            var devport = document.getElementById("dev_port");
            var devcport = document.getElementById("dev_cport");
            var devbaud = document.getElementById("dev_baud");
            var devparity = document.getElementById("dev_parity");
            var devdata = document.getElementById("dev_data");
            var devstop = document.getElementById("dev_stop");
            var devpause = document.getElementById("dev_pause");
            
            var distart = document.getElementById("di_start");
            var disize = document.getElementById("di_size");
            var dostart = document.getElementById("do_start");
            var dosize = document.getElementById("do_size");
            var aistart = document.getElementById("ai_start");
            var aisize = document.getElementById("ai_size");
            var aorstart = document.getElementById("aor_start");
            var aorsize = document.getElementById("aor_size");
            var aowstart = document.getElementById("aow_start");
            var aowsize = document.getElementById("aow_size");"""
