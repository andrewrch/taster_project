

//static void createVertexBuffer()
//{
//  glm::vec3 Vertices[4];
//  Vertices[0] = glm::vec3(-1.0f, -1.0f, 0.5773f);
//  Vertices[1] = glm::vec3(0.0f, -1.0f, -1.15475);
//  Vertices[2] = glm::vec3(1.0f, -1.0f, 0.5773f);
//  Vertices[3] = glm::vec3(0.0f, 1.0f, 0.0f);
//
// 	 glGenBuffers(1, &VBO);
//	 glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	 glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
//}
//
//static void createIndexBuffer()
//{
//  unsigned int indices[] = { 0, 3, 1,
//                1, 3, 2,
//                2, 3, 0,
//                0, 2, 1 };
//
//  glGenBuffers(1, &IBO);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//}
