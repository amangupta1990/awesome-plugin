#pragma once

namespace SVGButtons
{
    const char* addButtonSVG = R"(
        <svg width="14" height="14" viewBox="0 0 14 14" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M7 1V13M1 7H13" stroke="black" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
        </svg>
    )";

    const char* muteButtonOnSVG = R"(
        <svg width="66" height="24" viewBox="0 0 66 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="66" height="24" rx="3" fill="#00A985"/>
<path d="M16.3897 8.78359C17.1587 9.53876 17.6824 10.5008 17.8944 11.5481C18.1065 12.5954 17.9975 13.6809 17.5812 14.6674C17.1648 15.6539 16.4599 16.497 15.5554 17.0902C14.651 17.6834 13.5877 18 12.5 18C11.4123 18 10.349 17.6834 9.44455 17.0902C8.54012 16.497 7.83517 15.6539 7.41884 14.6674C7.0025 13.6809 6.89348 12.5954 7.10555 11.5481C7.31763 10.5008 7.84127 9.53876 8.61028 8.78359M12.5031 6V11.9991" stroke="black" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M27.6519 7.81818H29.1235L32.5837 16.2699H32.703L36.1632 7.81818H37.6348V18H36.4814V10.2642H36.382L33.2002 18H32.0865L28.9047 10.2642H28.8053V18H27.6519V7.81818ZM44.9003 14.8778V10.3636H46.0736V18H44.9003V16.7074H44.8208C44.6418 17.0952 44.3634 17.425 43.9856 17.6967C43.6077 17.9652 43.1305 18.0994 42.5538 18.0994C42.0765 18.0994 41.6522 17.995 41.281 17.7862C40.9098 17.5741 40.6182 17.2559 40.406 16.8317C40.1939 16.4041 40.0878 15.8655 40.0878 15.2159V10.3636H41.2611V15.1364C41.2611 15.6932 41.4169 16.1373 41.7285 16.4688C42.0433 16.8002 42.4444 16.9659 42.9316 16.9659C43.2233 16.9659 43.5199 16.8913 43.8215 16.7422C44.1264 16.593 44.3816 16.3643 44.5871 16.0561C44.7959 15.7479 44.9003 15.3551 44.9003 14.8778ZM51.6836 10.3636V11.358H47.7263V10.3636H51.6836ZM48.8797 8.53409H50.053V15.8125C50.053 16.1439 50.101 16.3925 50.1971 16.5582C50.2966 16.7206 50.4225 16.83 50.575 16.8864C50.7307 16.9394 50.8948 16.9659 51.0672 16.9659C51.1964 16.9659 51.3025 16.9593 51.3853 16.946C51.4682 16.9295 51.5345 16.9162 51.5842 16.9062L51.8228 17.9602C51.7433 17.9901 51.6323 18.0199 51.4897 18.0497C51.3472 18.0829 51.1666 18.0994 50.9478 18.0994C50.6164 18.0994 50.2916 18.0282 49.9734 17.8857C49.6585 17.7431 49.3967 17.526 49.1879 17.2344C48.9824 16.9427 48.8797 16.5748 48.8797 16.1307V8.53409ZM56.7082 18.1591C55.9724 18.1591 55.3377 17.9967 54.8041 17.6719C54.2738 17.3438 53.8645 16.8864 53.5761 16.2997C53.2911 15.7098 53.1486 15.0237 53.1486 14.2415C53.1486 13.4593 53.2911 12.7699 53.5761 12.1733C53.8645 11.5734 54.2655 11.1061 54.7792 10.7713C55.2963 10.4332 55.8995 10.2642 56.5889 10.2642C56.9866 10.2642 57.3794 10.3305 57.7672 10.4631C58.155 10.5956 58.5079 10.8111 58.8261 11.1094C59.1443 11.4044 59.3979 11.7955 59.5868 12.2827C59.7757 12.7699 59.8702 13.3698 59.8702 14.0824V14.5795H53.9838V13.5653H58.677C58.677 13.1345 58.5908 12.75 58.4185 12.4119C58.2494 12.0739 58.0075 11.8071 57.6926 11.6115C57.381 11.416 57.0131 11.3182 56.5889 11.3182C56.1216 11.3182 55.7172 11.4342 55.3758 11.6662C55.0378 11.8949 54.7776 12.1932 54.5953 12.5611C54.413 12.929 54.3219 13.3234 54.3219 13.7443V14.4205C54.3219 14.9972 54.4213 15.486 54.6202 15.8871C54.8223 16.2848 55.1024 16.5881 55.4604 16.7969C55.8183 17.0024 56.2343 17.1051 56.7082 17.1051C57.0165 17.1051 57.2949 17.062 57.5435 16.9759C57.7953 16.8864 58.0124 16.7538 58.1947 16.5781C58.377 16.3991 58.5179 16.1771 58.6173 15.9119L59.7508 16.2301C59.6315 16.6146 59.431 16.9527 59.1493 17.2443C58.8676 17.5327 58.5195 17.758 58.1052 17.9205C57.6909 18.0795 57.2253 18.1591 56.7082 18.1591Z" fill="black"/>
</svg>

    )";

    const char* muteButtonOffSVG = R"(
        <svg width="66" height="24" viewBox="0 0 66 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="66" height="24" rx="3" fill="#4C4C4C"/>
<path d="M16.3897 8.78359C17.1587 9.53876 17.6824 10.5008 17.8944 11.5481C18.1065 12.5954 17.9975 13.6809 17.5812 14.6674C17.1648 15.6539 16.4599 16.497 15.5554 17.0902C14.651 17.6834 13.5877 18 12.5 18C11.4123 18 10.349 17.6834 9.44455 17.0902C8.54012 16.497 7.83517 15.6539 7.41884 14.6674C7.0025 13.6809 6.89348 12.5954 7.10555 11.5481C7.31763 10.5008 7.84127 9.53876 8.61028 8.78359M12.5031 6V11.9991" stroke="black" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M27.6519 7.81818H29.1235L32.5837 16.2699H32.703L36.1632 7.81818H37.6348V18H36.4814V10.2642H36.382L33.2002 18H32.0865L28.9047 10.2642H28.8053V18H27.6519V7.81818ZM44.9003 14.8778V10.3636H46.0736V18H44.9003V16.7074H44.8208C44.6418 17.0952 44.3634 17.425 43.9856 17.6967C43.6077 17.9652 43.1305 18.0994 42.5538 18.0994C42.0765 18.0994 41.6522 17.995 41.281 17.7862C40.9098 17.5741 40.6182 17.2559 40.406 16.8317C40.1939 16.4041 40.0878 15.8655 40.0878 15.2159V10.3636H41.2611V15.1364C41.2611 15.6932 41.4169 16.1373 41.7285 16.4688C42.0433 16.8002 42.4444 16.9659 42.9316 16.9659C43.2233 16.9659 43.5199 16.8913 43.8215 16.7422C44.1264 16.593 44.3816 16.3643 44.5871 16.0561C44.7959 15.7479 44.9003 15.3551 44.9003 14.8778ZM51.6836 10.3636V11.358H47.7263V10.3636H51.6836ZM48.8797 8.53409H50.053V15.8125C50.053 16.1439 50.101 16.3925 50.1971 16.5582C50.2966 16.7206 50.4225 16.83 50.575 16.8864C50.7307 16.9394 50.8948 16.9659 51.0672 16.9659C51.1964 16.9659 51.3025 16.9593 51.3853 16.946C51.4682 16.9295 51.5345 16.9162 51.5842 16.9062L51.8228 17.9602C51.7433 17.9901 51.6323 18.0199 51.4897 18.0497C51.3472 18.0829 51.1666 18.0994 50.9478 18.0994C50.6164 18.0994 50.2916 18.0282 49.9734 17.8857C49.6585 17.7431 49.3967 17.526 49.1879 17.2344C48.9824 16.9427 48.8797 16.5748 48.8797 16.1307V8.53409ZM56.7082 18.1591C55.9724 18.1591 55.3377 17.9967 54.8041 17.6719C54.2738 17.3438 53.8645 16.8864 53.5761 16.2997C53.2911 15.7098 53.1486 15.0237 53.1486 14.2415C53.1486 13.4593 53.2911 12.7699 53.5761 12.1733C53.8645 11.5734 54.2655 11.1061 54.7792 10.7713C55.2963 10.4332 55.8995 10.2642 56.5889 10.2642C56.9866 10.2642 57.3794 10.3305 57.7672 10.4631C58.155 10.5956 58.5079 10.8111 58.8261 11.1094C59.1443 11.4044 59.3979 11.7955 59.5868 12.2827C59.7757 12.7699 59.8702 13.3698 59.8702 14.0824V14.5795H53.9838V13.5653H58.677C58.677 13.1345 58.5908 12.75 58.4185 12.4119C58.2494 12.0739 58.0075 11.8071 57.6926 11.6115C57.381 11.416 57.0131 11.3182 56.5889 11.3182C56.1216 11.3182 55.7172 11.4342 55.3758 11.6662C55.0378 11.8949 54.7776 12.1932 54.5953 12.5611C54.413 12.929 54.3219 13.3234 54.3219 13.7443V14.4205C54.3219 14.9972 54.4213 15.486 54.6202 15.8871C54.8223 16.2848 55.1024 16.5881 55.4604 16.7969C55.8183 17.0024 56.2343 17.1051 56.7082 17.1051C57.0165 17.1051 57.2949 17.062 57.5435 16.9759C57.7953 16.8864 58.0124 16.7538 58.1947 16.5781C58.377 16.3991 58.5179 16.1771 58.6173 15.9119L59.7508 16.2301C59.6315 16.6146 59.431 16.9527 59.1493 17.2443C58.8676 17.5327 58.5195 17.758 58.1052 17.9205C57.6909 18.0795 57.2253 18.1591 56.7082 18.1591Z" fill="black"/>
</svg>

    )";

    const char* bypassButtonOnSVG = R"(
       <svg width="82" height="24" viewBox="0 0 82 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="82" height="24" rx="3" fill="#00A985"/>
<path d="M16.3897 8.78359C17.1587 9.53876 17.6824 10.5008 17.8944 11.5481C18.1065 12.5954 17.9975 13.6809 17.5812 14.6674C17.1648 15.6539 16.4599 16.497 15.5554 17.0902C14.651 17.6834 13.5877 18 12.5 18C11.4123 18 10.349 17.6834 9.44455 17.0902C8.54012 16.497 7.83517 15.6539 7.41884 14.6674C7.0025 13.6809 6.89348 12.5954 7.10555 11.5481C7.31763 10.5008 7.84127 9.53876 8.61028 8.78359M12.5031 6V11.9991" stroke="black" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M29.3393 18V7.81818H32.899C33.6083 7.81818 34.1933 7.94081 34.654 8.18608C35.1147 8.42803 35.4577 8.7545 35.6831 9.16548C35.9085 9.57315 36.0211 10.0256 36.0211 10.5227C36.0211 10.9602 35.9433 11.3215 35.7875 11.6065C35.635 11.8916 35.4328 12.117 35.1809 12.2827C34.9324 12.4484 34.6622 12.571 34.3706 12.6506V12.75C34.6821 12.7699 34.9953 12.8793 35.3102 13.0781C35.6251 13.277 35.8886 13.562 36.1007 13.9332C36.3128 14.3045 36.4189 14.7585 36.4189 15.2955C36.4189 15.8059 36.3029 16.2649 36.0709 16.6726C35.8388 17.0803 35.4726 17.4034 34.9721 17.642C34.4717 17.8807 33.8204 18 33.0183 18H29.3393ZM30.5723 16.9062H33.0183C33.8237 16.9062 34.3954 16.7505 34.7335 16.4389C35.0749 16.1241 35.2456 15.7429 35.2456 15.2955C35.2456 14.9508 35.1577 14.6326 34.9821 14.3409C34.8064 14.0459 34.5562 13.8106 34.2314 13.6349C33.9066 13.456 33.5221 13.3665 33.078 13.3665H30.5723V16.9062ZM30.5723 12.2926H32.8592C33.2304 12.2926 33.5652 12.2197 33.8635 12.0739C34.1651 11.928 34.4037 11.7225 34.5794 11.4574C34.7584 11.1922 34.8478 10.8807 34.8478 10.5227C34.8478 10.0753 34.6921 9.69579 34.3805 9.38423C34.069 9.06937 33.5751 8.91193 32.899 8.91193H30.5723V12.2926ZM39.1814 20.8636C38.9825 20.8636 38.8052 20.8471 38.6494 20.8139C38.4936 20.7841 38.3859 20.7543 38.3263 20.7244L38.6246 19.6903C38.9096 19.7633 39.1615 19.7898 39.3802 19.7699C39.599 19.75 39.7929 19.6522 39.9619 19.4766C40.1343 19.3042 40.2917 19.0241 40.4342 18.6364L40.653 18.0398L37.8291 10.3636H39.1018L41.2098 16.4489H41.2893L43.3973 10.3636H44.67L41.4285 19.1136C41.2827 19.508 41.1021 19.8345 40.8866 20.093C40.6712 20.3549 40.421 20.5488 40.1359 20.6747C39.8542 20.8007 39.536 20.8636 39.1814 20.8636ZM46.3587 20.8636V10.3636H47.4922V11.5767H47.6314C47.7176 11.4441 47.8369 11.2751 47.9894 11.0696C48.1451 10.8608 48.3672 10.6752 48.6555 10.5128C48.9472 10.3471 49.3416 10.2642 49.8388 10.2642C50.4818 10.2642 51.0485 10.425 51.5391 10.7464C52.0296 11.0679 52.4124 11.5237 52.6875 12.1136C52.9626 12.7036 53.1001 13.3996 53.1001 14.2017C53.1001 15.0104 52.9626 15.7114 52.6875 16.3047C52.4124 16.8946 52.0313 17.352 51.544 17.6768C51.0568 17.9983 50.495 18.1591 49.8587 18.1591C49.3681 18.1591 48.9754 18.0779 48.6804 17.9155C48.3854 17.7498 48.1584 17.5625 47.9993 17.3537C47.8402 17.1416 47.7176 16.9659 47.6314 16.8267H47.532V20.8636H46.3587ZM47.5121 14.1818C47.5121 14.7585 47.5966 15.2673 47.7656 15.7081C47.9347 16.1456 48.1816 16.4886 48.5064 16.7372C48.8312 16.9825 49.2289 17.1051 49.6996 17.1051C50.1901 17.1051 50.5994 16.9759 50.9276 16.7173C51.259 16.4555 51.5076 16.1042 51.6733 15.6634C51.8423 15.2192 51.9269 14.7254 51.9269 14.1818C51.9269 13.6449 51.844 13.161 51.6783 12.7301C51.5159 12.2959 51.2689 11.9529 50.9375 11.701C50.6094 11.4458 50.1967 11.3182 49.6996 11.3182C49.2223 11.3182 48.8213 11.4392 48.4965 11.6811C48.1716 11.9197 47.9264 12.2545 47.7607 12.6854C47.5949 13.1129 47.5121 13.6117 47.5121 14.1818ZM57.2771 18.179C56.7932 18.179 56.354 18.0878 55.9596 17.9055C55.5652 17.7199 55.252 17.4531 55.02 17.1051C54.788 16.7538 54.672 16.3295 54.672 15.8324C54.672 15.3949 54.7581 15.0402 54.9305 14.7685C55.1028 14.4934 55.3332 14.2779 55.6215 14.1222C55.9099 13.9664 56.2281 13.8504 56.5761 13.7741C56.9274 13.6946 57.2804 13.6316 57.635 13.5852C58.099 13.5256 58.4752 13.4808 58.7636 13.451C59.0553 13.4179 59.2674 13.3632 59.3999 13.2869C59.5358 13.2107 59.6038 13.0781 59.6038 12.8892V12.8494C59.6038 12.3589 59.4695 11.9777 59.2011 11.706C58.9359 11.4342 58.5332 11.2983 57.993 11.2983C57.4329 11.2983 56.9937 11.4209 56.6755 11.6662C56.3573 11.9115 56.1336 12.1733 56.0044 12.4517L54.8907 12.054C55.0896 11.59 55.3547 11.2287 55.6862 10.9702C56.0209 10.7083 56.3855 10.526 56.7799 10.4233C57.1776 10.3172 57.5687 10.2642 57.9532 10.2642C58.1985 10.2642 58.4802 10.294 58.7984 10.3537C59.1199 10.41 59.4298 10.5277 59.7281 10.7067C60.0297 10.8857 60.2799 11.1558 60.4788 11.517C60.6776 11.8783 60.7771 12.3622 60.7771 12.9688V18H59.6038V16.9659H59.5441C59.4646 17.1316 59.332 17.3089 59.1464 17.4979C58.9608 17.6868 58.7139 17.8475 58.4056 17.9801C58.0974 18.1127 57.7212 18.179 57.2771 18.179ZM57.4561 17.125C57.9201 17.125 58.3112 17.0339 58.6294 16.8516C58.9508 16.6693 59.1928 16.4339 59.3552 16.1456C59.5209 15.8572 59.6038 15.554 59.6038 15.2358V14.1619C59.5541 14.2216 59.4447 14.2763 59.2757 14.326C59.1099 14.3724 58.9177 14.4138 58.699 14.4503C58.4835 14.4834 58.2731 14.5133 58.0676 14.5398C57.8654 14.563 57.7013 14.5829 57.5754 14.5994C57.2704 14.6392 56.9854 14.7038 56.7203 14.7933C56.4584 14.8795 56.2463 15.0104 56.0839 15.1861C55.9248 15.3584 55.8453 15.5937 55.8453 15.892C55.8453 16.2997 55.9961 16.608 56.2977 16.8168C56.6026 17.0223 56.9887 17.125 57.4561 17.125ZM68.4677 12.0739L67.4137 12.3722C67.3474 12.1965 67.2496 12.0258 67.1204 11.8601C66.9944 11.6911 66.8221 11.5518 66.6033 11.4425C66.3846 11.3331 66.1045 11.2784 65.7631 11.2784C65.2958 11.2784 64.9064 11.3861 64.5948 11.6016C64.2866 11.8137 64.1325 12.0838 64.1325 12.4119C64.1325 12.7036 64.2385 12.9339 64.4506 13.103C64.6628 13.272 64.9942 13.4129 65.445 13.5256L66.5785 13.804C67.2612 13.9697 67.77 14.2232 68.1048 14.5646C68.4395 14.9027 68.6069 15.3385 68.6069 15.8722C68.6069 16.3097 68.4809 16.7008 68.229 17.0455C67.9805 17.3902 67.6325 17.6619 67.185 17.8608C66.7376 18.0597 66.2172 18.1591 65.6239 18.1591C64.845 18.1591 64.2004 17.9901 63.69 17.652C63.1796 17.3139 62.8564 16.8201 62.7205 16.1705L63.8342 15.892C63.9402 16.303 64.1407 16.6113 64.4357 16.8168C64.734 17.0223 65.1235 17.125 65.604 17.125C66.1509 17.125 66.5851 17.009 66.9066 16.777C67.2314 16.5417 67.3938 16.2599 67.3938 15.9318C67.3938 15.6667 67.301 15.4446 67.1154 15.2656C66.9298 15.0833 66.6448 14.9474 66.2603 14.858L64.9876 14.5597C64.2882 14.3939 63.7745 14.1371 63.4464 13.7891C63.1216 13.4377 62.9592 12.9986 62.9592 12.4716C62.9592 12.0407 63.0801 11.6596 63.3221 11.3281C63.5673 10.9967 63.9004 10.7365 64.3214 10.5476C64.7456 10.3587 65.2262 10.2642 65.7631 10.2642C66.5188 10.2642 67.1121 10.4299 67.543 10.7614C67.9772 11.0928 68.2854 11.5303 68.4677 12.0739ZM75.9221 12.0739L74.8682 12.3722C74.8019 12.1965 74.7041 12.0258 74.5748 11.8601C74.4489 11.6911 74.2765 11.5518 74.0578 11.4425C73.839 11.3331 73.559 11.2784 73.2176 11.2784C72.7503 11.2784 72.3608 11.3861 72.0493 11.6016C71.741 11.8137 71.5869 12.0838 71.5869 12.4119C71.5869 12.7036 71.693 12.9339 71.9051 13.103C72.1172 13.272 72.4486 13.4129 72.8994 13.5256L74.0329 13.804C74.7157 13.9697 75.2245 14.2232 75.5592 14.5646C75.894 14.9027 76.0613 15.3385 76.0613 15.8722C76.0613 16.3097 75.9354 16.7008 75.6835 17.0455C75.4349 17.3902 75.0869 17.6619 74.6395 17.8608C74.192 18.0597 73.6717 18.1591 73.0784 18.1591C72.2995 18.1591 71.6549 17.9901 71.1444 17.652C70.634 17.3139 70.3109 16.8201 70.175 16.1705L71.2886 15.892C71.3947 16.303 71.5952 16.6113 71.8902 16.8168C72.1885 17.0223 72.5779 17.125 73.0585 17.125C73.6054 17.125 74.0396 17.009 74.3611 16.777C74.6859 16.5417 74.8483 16.2599 74.8483 15.9318C74.8483 15.6667 74.7555 15.4446 74.5699 15.2656C74.3843 15.0833 74.0992 14.9474 73.7147 14.858L72.442 14.5597C71.7427 14.3939 71.229 14.1371 70.9008 13.7891C70.576 13.4377 70.4136 12.9986 70.4136 12.4716C70.4136 12.0407 70.5346 11.6596 70.7765 11.3281C71.0218 10.9967 71.3549 10.7365 71.7758 10.5476C72.2001 10.3587 72.6807 10.2642 73.2176 10.2642C73.9733 10.2642 74.5665 10.4299 74.9974 10.7614C75.4316 11.0928 75.7398 11.5303 75.9221 12.0739Z" fill="black"/>
</svg>

    )";

    const char* bypassButtonOffSVG = R"(
<svg width="82" height="24" viewBox="0 0 82 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="82" height="24" rx="3" fill="#4C4C4C"/>
<path d="M16.3897 8.78359C17.1587 9.53876 17.6824 10.5008 17.8944 11.5481C18.1065 12.5954 17.9975 13.6809 17.5812 14.6674C17.1648 15.6539 16.4599 16.497 15.5554 17.0902C14.651 17.6834 13.5877 18 12.5 18C11.4123 18 10.349 17.6834 9.44455 17.0902C8.54012 16.497 7.83517 15.6539 7.41884 14.6674C7.0025 13.6809 6.89348 12.5954 7.10555 11.5481C7.31763 10.5008 7.84127 9.53876 8.61028 8.78359M12.5031 6V11.9991" stroke="black" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M29.3393 18V7.81818H32.899C33.6083 7.81818 34.1933 7.94081 34.654 8.18608C35.1147 8.42803 35.4577 8.7545 35.6831 9.16548C35.9085 9.57315 36.0211 10.0256 36.0211 10.5227C36.0211 10.9602 35.9433 11.3215 35.7875 11.6065C35.635 11.8916 35.4328 12.117 35.1809 12.2827C34.9324 12.4484 34.6622 12.571 34.3706 12.6506V12.75C34.6821 12.7699 34.9953 12.8793 35.3102 13.0781C35.6251 13.277 35.8886 13.562 36.1007 13.9332C36.3128 14.3045 36.4189 14.7585 36.4189 15.2955C36.4189 15.8059 36.3029 16.2649 36.0709 16.6726C35.8388 17.0803 35.4726 17.4034 34.9721 17.642C34.4717 17.8807 33.8204 18 33.0183 18H29.3393ZM30.5723 16.9062H33.0183C33.8237 16.9062 34.3954 16.7505 34.7335 16.4389C35.0749 16.1241 35.2456 15.7429 35.2456 15.2955C35.2456 14.9508 35.1577 14.6326 34.9821 14.3409C34.8064 14.0459 34.5562 13.8106 34.2314 13.6349C33.9066 13.456 33.5221 13.3665 33.078 13.3665H30.5723V16.9062ZM30.5723 12.2926H32.8592C33.2304 12.2926 33.5652 12.2197 33.8635 12.0739C34.1651 11.928 34.4037 11.7225 34.5794 11.4574C34.7584 11.1922 34.8478 10.8807 34.8478 10.5227C34.8478 10.0753 34.6921 9.69579 34.3805 9.38423C34.069 9.06937 33.5751 8.91193 32.899 8.91193H30.5723V12.2926ZM39.1814 20.8636C38.9825 20.8636 38.8052 20.8471 38.6494 20.8139C38.4936 20.7841 38.3859 20.7543 38.3263 20.7244L38.6246 19.6903C38.9096 19.7633 39.1615 19.7898 39.3802 19.7699C39.599 19.75 39.7929 19.6522 39.9619 19.4766C40.1343 19.3042 40.2917 19.0241 40.4342 18.6364L40.653 18.0398L37.8291 10.3636H39.1018L41.2098 16.4489H41.2893L43.3973 10.3636H44.67L41.4285 19.1136C41.2827 19.508 41.1021 19.8345 40.8866 20.093C40.6712 20.3549 40.421 20.5488 40.1359 20.6747C39.8542 20.8007 39.536 20.8636 39.1814 20.8636ZM46.3587 20.8636V10.3636H47.4922V11.5767H47.6314C47.7176 11.4441 47.8369 11.2751 47.9894 11.0696C48.1451 10.8608 48.3672 10.6752 48.6555 10.5128C48.9472 10.3471 49.3416 10.2642 49.8388 10.2642C50.4818 10.2642 51.0485 10.425 51.5391 10.7464C52.0296 11.0679 52.4124 11.5237 52.6875 12.1136C52.9626 12.7036 53.1001 13.3996 53.1001 14.2017C53.1001 15.0104 52.9626 15.7114 52.6875 16.3047C52.4124 16.8946 52.0313 17.352 51.544 17.6768C51.0568 17.9983 50.495 18.1591 49.8587 18.1591C49.3681 18.1591 48.9754 18.0779 48.6804 17.9155C48.3854 17.7498 48.1584 17.5625 47.9993 17.3537C47.8402 17.1416 47.7176 16.9659 47.6314 16.8267H47.532V20.8636H46.3587ZM47.5121 14.1818C47.5121 14.7585 47.5966 15.2673 47.7656 15.7081C47.9347 16.1456 48.1816 16.4886 48.5064 16.7372C48.8312 16.9825 49.2289 17.1051 49.6996 17.1051C50.1901 17.1051 50.5994 16.9759 50.9276 16.7173C51.259 16.4555 51.5076 16.1042 51.6733 15.6634C51.8423 15.2192 51.9269 14.7254 51.9269 14.1818C51.9269 13.6449 51.844 13.161 51.6783 12.7301C51.5159 12.2959 51.2689 11.9529 50.9375 11.701C50.6094 11.4458 50.1967 11.3182 49.6996 11.3182C49.2223 11.3182 48.8213 11.4392 48.4965 11.6811C48.1716 11.9197 47.9264 12.2545 47.7607 12.6854C47.5949 13.1129 47.5121 13.6117 47.5121 14.1818ZM57.2771 18.179C56.7932 18.179 56.354 18.0878 55.9596 17.9055C55.5652 17.7199 55.252 17.4531 55.02 17.1051C54.788 16.7538 54.672 16.3295 54.672 15.8324C54.672 15.3949 54.7581 15.0402 54.9305 14.7685C55.1028 14.4934 55.3332 14.2779 55.6215 14.1222C55.9099 13.9664 56.2281 13.8504 56.5761 13.7741C56.9274 13.6946 57.2804 13.6316 57.635 13.5852C58.099 13.5256 58.4752 13.4808 58.7636 13.451C59.0553 13.4179 59.2674 13.3632 59.3999 13.2869C59.5358 13.2107 59.6038 13.0781 59.6038 12.8892V12.8494C59.6038 12.3589 59.4695 11.9777 59.2011 11.706C58.9359 11.4342 58.5332 11.2983 57.993 11.2983C57.4329 11.2983 56.9937 11.4209 56.6755 11.6662C56.3573 11.9115 56.1336 12.1733 56.0044 12.4517L54.8907 12.054C55.0896 11.59 55.3547 11.2287 55.6862 10.9702C56.0209 10.7083 56.3855 10.526 56.7799 10.4233C57.1776 10.3172 57.5687 10.2642 57.9532 10.2642C58.1985 10.2642 58.4802 10.294 58.7984 10.3537C59.1199 10.41 59.4298 10.5277 59.7281 10.7067C60.0297 10.8857 60.2799 11.1558 60.4788 11.517C60.6776 11.8783 60.7771 12.3622 60.7771 12.9688V18H59.6038V16.9659H59.5441C59.4646 17.1316 59.332 17.3089 59.1464 17.4979C58.9608 17.6868 58.7139 17.8475 58.4056 17.9801C58.0974 18.1127 57.7212 18.179 57.2771 18.179ZM57.4561 17.125C57.9201 17.125 58.3112 17.0339 58.6294 16.8516C58.9508 16.6693 59.1928 16.4339 59.3552 16.1456C59.5209 15.8572 59.6038 15.554 59.6038 15.2358V14.1619C59.5541 14.2216 59.4447 14.2763 59.2757 14.326C59.1099 14.3724 58.9177 14.4138 58.699 14.4503C58.4835 14.4834 58.2731 14.5133 58.0676 14.5398C57.8654 14.563 57.7013 14.5829 57.5754 14.5994C57.2704 14.6392 56.9854 14.7038 56.7203 14.7933C56.4584 14.8795 56.2463 15.0104 56.0839 15.1861C55.9248 15.3584 55.8453 15.5937 55.8453 15.892C55.8453 16.2997 55.9961 16.608 56.2977 16.8168C56.6026 17.0223 56.9887 17.125 57.4561 17.125ZM68.4677 12.0739L67.4137 12.3722C67.3474 12.1965 67.2496 12.0258 67.1204 11.8601C66.9944 11.6911 66.8221 11.5518 66.6033 11.4425C66.3846 11.3331 66.1045 11.2784 65.7631 11.2784C65.2958 11.2784 64.9064 11.3861 64.5948 11.6016C64.2866 11.8137 64.1325 12.0838 64.1325 12.4119C64.1325 12.7036 64.2385 12.9339 64.4506 13.103C64.6628 13.272 64.9942 13.4129 65.445 13.5256L66.5785 13.804C67.2612 13.9697 67.77 14.2232 68.1048 14.5646C68.4395 14.9027 68.6069 15.3385 68.6069 15.8722C68.6069 16.3097 68.4809 16.7008 68.229 17.0455C67.9805 17.3902 67.6325 17.6619 67.185 17.8608C66.7376 18.0597 66.2172 18.1591 65.6239 18.1591C64.845 18.1591 64.2004 17.9901 63.69 17.652C63.1796 17.3139 62.8564 16.8201 62.7205 16.1705L63.8342 15.892C63.9402 16.303 64.1407 16.6113 64.4357 16.8168C64.734 17.0223 65.1235 17.125 65.604 17.125C66.1509 17.125 66.5851 17.009 66.9066 16.777C67.2314 16.5417 67.3938 16.2599 67.3938 15.9318C67.3938 15.6667 67.301 15.4446 67.1154 15.2656C66.9298 15.0833 66.6448 14.9474 66.2603 14.858L64.9876 14.5597C64.2882 14.3939 63.7745 14.1371 63.4464 13.7891C63.1216 13.4377 62.9592 12.9986 62.9592 12.4716C62.9592 12.0407 63.0801 11.6596 63.3221 11.3281C63.5673 10.9967 63.9004 10.7365 64.3214 10.5476C64.7456 10.3587 65.2262 10.2642 65.7631 10.2642C66.5188 10.2642 67.1121 10.4299 67.543 10.7614C67.9772 11.0928 68.2854 11.5303 68.4677 12.0739ZM75.9221 12.0739L74.8682 12.3722C74.8019 12.1965 74.7041 12.0258 74.5748 11.8601C74.4489 11.6911 74.2765 11.5518 74.0578 11.4425C73.839 11.3331 73.559 11.2784 73.2176 11.2784C72.7503 11.2784 72.3608 11.3861 72.0493 11.6016C71.741 11.8137 71.5869 12.0838 71.5869 12.4119C71.5869 12.7036 71.693 12.9339 71.9051 13.103C72.1172 13.272 72.4486 13.4129 72.8994 13.5256L74.0329 13.804C74.7157 13.9697 75.2245 14.2232 75.5592 14.5646C75.894 14.9027 76.0613 15.3385 76.0613 15.8722C76.0613 16.3097 75.9354 16.7008 75.6835 17.0455C75.4349 17.3902 75.0869 17.6619 74.6395 17.8608C74.192 18.0597 73.6717 18.1591 73.0784 18.1591C72.2995 18.1591 71.6549 17.9901 71.1444 17.652C70.634 17.3139 70.3109 16.8201 70.175 16.1705L71.2886 15.892C71.3947 16.303 71.5952 16.6113 71.8902 16.8168C72.1885 17.0223 72.5779 17.125 73.0585 17.125C73.6054 17.125 74.0396 17.009 74.3611 16.777C74.6859 16.5417 74.8483 16.2599 74.8483 15.9318C74.8483 15.6667 74.7555 15.4446 74.5699 15.2656C74.3843 15.0833 74.0992 14.9474 73.7147 14.858L72.442 14.5597C71.7427 14.3939 71.229 14.1371 70.9008 13.7891C70.576 13.4377 70.4136 12.9986 70.4136 12.4716C70.4136 12.0407 70.5346 11.6596 70.7765 11.3281C71.0218 10.9967 71.3549 10.7365 71.7758 10.5476C72.2001 10.3587 72.6807 10.2642 73.2176 10.2642C73.9733 10.2642 74.5665 10.4299 74.9974 10.7614C75.4316 11.0928 75.7398 11.5303 75.9221 12.0739Z" fill="black"/>
</svg>

    )";

    const char* deleteButtonSVG = R"(
        <svg width="12" height="12" viewBox="0 0 12 12" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M11 1L6 6M6 6L1 11M6 6L1 1M6 6L11 11" stroke="white" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
        </svg>
    )";

    const char* expandButtonSVG = R"(
        <svg width="9" height="12" viewBox="0 0 9 12" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M1 4.5L4.5 1L8 4.5" stroke="white"/>
        <path d="M1 7.5L4.5 11L8 7.5" stroke="white"/>
        </svg>
    )";

    const char* closeButtonSVG = R"(
        <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
        <line x1="12" y1="12" x2="28" y2="28" stroke="white" stroke-width="2"/>
        <line x1="12" y1="28" x2="28" y2="12" stroke="white" stroke-width="2"/>
        </svg>
    )";


}


